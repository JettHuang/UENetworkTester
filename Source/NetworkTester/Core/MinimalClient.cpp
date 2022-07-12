// Copyright Epic Games, Inc. All Rights Reserved.
//

#include "MinimalClient.h"

#include "Misc/FeedbackContext.h"
#include "Misc/NetworkVersion.h"
#include "Engine/Engine.h"
#include "Engine/GameEngine.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/Actor.h"
#include "Net/DataChannel.h"
#include "EngineUtils.h"
#include "MyActorChannel.h"
#include "MyChatChannel.h"
#include "MyPackageMap.h"


DEFINE_LOG_CATEGORY(LogNetworkTester);


UMinimalClient::UMinimalClient(const FObjectInitializer& ObjectInitializor)
	: Super(ObjectInitializor)
	, Timeout(5)
	, UnitWorld(NULL)
	, UnitNetDriver(NULL)
{

}

void UMinimalClient::Tick(float DeltaTime)
{
	if (UnitNetDriver)
	{
		UnitNetDriver->TickDispatch(DeltaTime);
		UnitNetDriver->PostTickDispatch();

		UnitNetDriver->TickFlush(DeltaTime);
		UnitNetDriver->PostTickFlush();
	}
}

TStatId UMinimalClient::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMinimalClient, STATGROUP_Tickables);
}

bool UMinimalClient::Listen(const FString& InServerAddr, uint16 InPort)
{
	bool bSuccess = false;

	UnitWorld = CreateWorld();
	check(UnitWorld != NULL);

	UnitNetDriver = CreateNetDriver();
	check(UnitNetDriver != NULL);

	if (!UnitNetDriver) {
		UE_LOG(LogNetworkTester, Warning, TEXT("Error to create an instance of the unit test net driver."));
		return false;
	}

	// Hack: Replace the control and actor channels, with stripped down unit test channels
	UnitNetDriver->ChannelDefinitionMap[NAME_Actor].ChannelClass = UMyActorChannel::StaticClass();
	UnitNetDriver->ChannelDefinitionMap[NAME_Voice].ChannelClass = UMyChatChannel::StaticClass();

	UnitNetDriver->InitialConnectTimeout = FMath::Max(UnitNetDriver->InitialConnectTimeout, (float)Timeout);
	UnitNetDriver->ConnectionTimeout = FMath::Max(UnitNetDriver->ConnectionTimeout, (float)Timeout);

	FString ListenError;
	FURL ServerURL;
	ServerURL.Host = InServerAddr;
	ServerURL.Port = InPort;

	bSuccess = UnitNetDriver->InitListen(this, ServerURL, false, ListenError);
	return bSuccess;
}

bool UMinimalClient::Connect(const FString& InServerAddr, uint16 InPort)
{
	bool bSuccess = false;

	UnitWorld = CreateWorld();
	check(UnitWorld != NULL);

	UnitNetDriver = CreateNetDriver();
	check(UnitNetDriver != NULL);

	if (!UnitNetDriver) {
		UE_LOG(LogNetworkTester, Warning, TEXT("Error to create an instance of the unit test net driver."));
		return false;
	}

	UnitNetDriver->SetWorld(UnitWorld);

	// Hack: Replace the control and actor channels, with stripped down unit test channels
	UnitNetDriver->ChannelDefinitionMap[NAME_Actor].ChannelClass = UMyActorChannel::StaticClass();
	UnitNetDriver->ChannelDefinitionMap[NAME_Voice].ChannelClass = UMyChatChannel::StaticClass();
	
	UnitNetDriver->InitialConnectTimeout = FMath::Max(UnitNetDriver->InitialConnectTimeout, (float)Timeout);
	UnitNetDriver->ConnectionTimeout = FMath::Max(UnitNetDriver->ConnectionTimeout, (float)Timeout);

	FString ConnectionError;
	UClass* NetConnClass = UnitNetDriver->NetConnectionClass;
	UNetConnection* DefConn = NetConnClass != nullptr ? Cast<UNetConnection>(NetConnClass->GetDefaultObject()) : nullptr;

	FURL TravelURL;
	TravelURL.Host = InServerAddr;
	TravelURL.Port = InPort;
	bSuccess = DefConn != nullptr;
	if (bSuccess)
	{
		// Replace the package map class
		TSubclassOf<UPackageMap> OldClass = DefConn->PackageMapClass;
		FProperty* OldPostConstructLink = NetConnClass->PostConstructLink;
		FProperty* PackageMapProp = FindFieldChecked<FProperty>(NetConnClass, TEXT("PackageMapClass"));

		// Hack - force property initialization for the PackageMapClass property, so changing its default value works.
		check(PackageMapProp != nullptr && PackageMapProp->PostConstructLinkNext == nullptr);

		PackageMapProp->PostConstructLinkNext = NetConnClass->PostConstructLink;
		NetConnClass->PostConstructLink = PackageMapProp;
		DefConn->PackageMapClass = UMyPackageMapClient::StaticClass();

		bSuccess = UnitNetDriver->InitConnect(this, TravelURL, ConnectionError);

		DefConn->PackageMapClass = OldClass;
		NetConnClass->PostConstructLink = OldPostConstructLink;
		PackageMapProp->PostConstructLinkNext = nullptr;
	}
	else
	{
		UE_LOG(LogNetworkTester, Error, TEXT("Error to replace PackageMapClass, minimal client connection failed."));
	}

	if (bSuccess)
	{
		UNetConnection *UnitConn = UnitNetDriver->ServerConnection;

		check(UnitConn != nullptr);
		check(UnitConn->PackageMapClass == UMyPackageMapClient::StaticClass());

		UMyPackageMapClient* PackageMap = CastChecked<UMyPackageMapClient>(UnitConn->PackageMap);

		PackageMap->MinClient = this;

		FString LogMsg = FString::Printf(TEXT("Successfully created minimal client connection to IP '%s'"), *InServerAddr);

		UE_LOG(LogNetworkTester, Log, TEXT("%s"), *LogMsg);

		// Work around a minor UNetConnection bug, where QueuedBits is not initialized, until after the first Tick
		UnitConn->QueuedBits = -(MAX_PACKET_SIZE * 8);

		int ChannelIndex = UnitNetDriver->ChannelDefinitionMap[NAME_Voice].StaticChannelIndex;
		UMyChatChannel* UnitChatChan = CastChecked<UMyChatChannel>(UnitConn->Channels[ChannelIndex]);

		UnitChatChan->MinClient = this;

		if (UnitConn->Handler.IsValid())
		{
			UnitConn->Handler->BeginHandshaking(
				FPacketHandlerHandshakeComplete::CreateUObject(this, &UMinimalClient::SendInitialJoin));
		}
		else
		{
			SendInitialJoin();
		}
	}
	else
	{
		UE_LOG(LogNetworkTester, Error, TEXT("Error to kickoff connect to IP '%s', error: %s"), *InServerAddr,
			*ConnectionError);
	}


	return bSuccess;
}

void UMinimalClient::SendInitialJoin()
{
	if (UnitNetDriver != nullptr)
	{
		UNetConnection* ServerConn = UnitNetDriver->ServerConnection;

		if (ServerConn != nullptr)
		{
			uint8 IsLittleEndian = uint8(PLATFORM_LITTLE_ENDIAN);
			check(IsLittleEndian == !!IsLittleEndian); // should only be one or zero

			uint32 LocalNetworkVersion = FNetworkVersion::GetLocalNetworkVersion();

			UE_LOG(LogNet, Log, TEXT("UPendingNetGame::SendInitialJoin: Sending hello. %s"), *ServerConn->Describe());

			FString EncryptionToken;

			FNetControlMessage<NMT_Hello>::Send(ServerConn, IsLittleEndian, LocalNetworkVersion, EncryptionToken);

			ServerConn->FlushNet();
		}
	}

	UE_LOG(LogNetworkTester, Warning, TEXT("Handshake successfully with server!"));
}

void UMinimalClient::Cleanup()
{
	if (UnitNetDriver)
	{
		UnitNetDriver->SetWorld(NULL);
		CleanupNetDriver(UnitNetDriver);
		UnitNetDriver = NULL;
	}

	if (UnitWorld)
	{
		CleanupWorld(UnitWorld);
		UnitWorld = NULL;
	}

	// Immediately garbage collect remaining objects, to finish net driver cleanup
	CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS, true);
}

void UMinimalClient::SendText(FString& InText)
{
	int ChannelIndex = UnitNetDriver->ChannelDefinitionMap[NAME_Voice].StaticChannelIndex;

	if (UnitNetDriver->ServerConnection)
	{
		UNetConnection* UnitConn = UnitNetDriver->ServerConnection;
		UMyChatChannel* UnitChatChan = CastChecked<UMyChatChannel>(UnitConn->Channels[ChannelIndex]);
		FOutBunch OutBunch(UnitChatChan, false);

		OutBunch.bReliable = 1;
		OutBunch << InText;
		UnitChatChan->SendBunch(&OutBunch, false);
	}
	else
	{
		for (auto UnitConn : UnitNetDriver->ClientConnections)
		{
			UMyChatChannel* UnitChatChan = CastChecked<UMyChatChannel>(UnitConn->Channels[ChannelIndex]);
			FOutBunch OutBunch(UnitChatChan, false);

			OutBunch.bReliable = 1;
			OutBunch << InText;
			UnitChatChan->SendBunch(&OutBunch, false);

			UnitConn->FlushNet();
		}
	}
}

void UMinimalClient::ResetConnTimeout(float Duration)
{

}

UWorld* UMinimalClient::CreateWorld()
{
	UWorld* ReturnVal = NULL;

	ReturnVal = UWorld::CreateWorld(EWorldType::None, false);

	// Hack-mark the world as having begun play (when it has not)
	ReturnVal->bBegunPlay = true;

	// Hack-mark the world as having initialized actors (to allow RPC hooks)
	ReturnVal->bActorsInitialized = true;

	// Create a blank world context, to prevent crashes
	FWorldContext& CurContext = GEngine->CreateNewWorldContext(EWorldType::None);
	CurContext.SetCurrentWorld(ReturnVal);

	return ReturnVal;
}

void UMinimalClient::CleanupWorld(UWorld* InWorld)
{
	// Iterate all ActorComponents in this world, and unmark them as having begun play - to prevent a crash during GC
	for (TActorIterator<AActor> ActorIt(InWorld); ActorIt; ++ActorIt)
	{
		for (UActorComponent* CurComp : ActorIt->GetComponents())
		{
			if (CurComp->HasBegunPlay())
			{
				// Big hack - call only the parent class UActorComponent::EndPlay function, such that only bHasBegunPlay is unset
				bool bBeginDestroyed = CurComp->HasAnyFlags(RF_BeginDestroyed);

				CurComp->SetFlags(RF_BeginDestroyed);

				CurComp->UActorComponent::EndPlay(EEndPlayReason::Quit);

				if (!bBeginDestroyed)
				{
					CurComp->ClearFlags(RF_BeginDestroyed);
				}
			}
		}
	} // end for 

	GEngine->DestroyWorldContext(InWorld);
	InWorld->DestroyWorld(false);
}

static int UnitTestNetDriverCount = 0;

UNetDriver* UMinimalClient::CreateNetDriver()
{
	UNetDriver* ReturnVal = NULL;

	const FString DriverClassName = TEXT("/Script/OnlineSubsystemUtils.IpNetDriver");
	UEngine* Engine = GEngine;
	if (Engine != nullptr && UnitWorld != nullptr)
	{
		// Setup a new driver name entry
		bool bFoundDef = false;
		
		int32 ClassLoc = DriverClassName.Find(TEXT("."));
		FName UnitDefName = *(FString(TEXT("NetworkingTester_")) + DriverClassName.Mid(ClassLoc + 1));

		for (int32 i = 0; i < Engine->NetDriverDefinitions.Num(); i++)
		{
			if (Engine->NetDriverDefinitions[i].DefName == UnitDefName)
			{
				bFoundDef = true;
				break;
			}
		} // end for i

		if (!bFoundDef)
		{
			FNetDriverDefinition NewDriverEntry;

			NewDriverEntry.DefName = UnitDefName;
			NewDriverEntry.DriverClassName = FName(*DriverClassName);
			// Don't allow fallbacks for the MinimalClient
			NewDriverEntry.DriverClassNameFallback = NewDriverEntry.DriverClassName;

			Engine->NetDriverDefinitions.Add(NewDriverEntry);
		}


		FName NewDriverName = *FString::Printf(TEXT("NetworkingTester_NetDriver_%i"), UnitTestNetDriverCount++);

		// Now create a reference to the driver
		if (Engine->CreateNamedNetDriver(UnitWorld, NewDriverName, UnitDefName))
		{
			ReturnVal = Engine->FindNamedNetDriver(UnitWorld, NewDriverName);
		}


		if (ReturnVal != nullptr)
		{
			ReturnVal->SetWorld(UnitWorld);
			UnitWorld->SetNetDriver(ReturnVal);

			ReturnVal->InitConnectionClass();


			FLevelCollection* Collection = (FLevelCollection*)UnitWorld->GetActiveLevelCollection();

			// Hack-set the net driver in the worlds level collection
			if (Collection != nullptr)
			{
				Collection->SetNetDriver(ReturnVal);
			}
			else
			{
				UE_LOG(LogNetworkTester, Warning, TEXT("CreateNetDriver: No LevelCollection found for created world, may block replication."));
			}

			UE_LOG(LogNetworkTester, Warning, TEXT("CreateNetDriver: Created named net driver: %s, NetDriverName: %s, for World: %s"),
				*ReturnVal->GetFullName(), *ReturnVal->NetDriverName.ToString(), *UnitWorld->GetFullName());
		}
		else
		{
			UE_LOG(LogNetworkTester, Warning, TEXT("CreateNetDriver: CreateNamedNetDriver failed"));
		}
	}
	else if (Engine == nullptr)
	{
		UE_LOG(LogNetworkTester, Warning, TEXT("CreateNetDriver: Engine is nullptr"));
	}
	else //if (UnitWorld == nullptr)
	{
		UE_LOG(LogNetworkTester, Warning, TEXT("CreateNetDriver: UnitWorld is nullptr"));
	}

	return ReturnVal;
}

void UMinimalClient::CleanupNetDriver(UNetDriver* InDriver)
{
	if (UnitWorld != nullptr && InDriver != nullptr)
	{
		GEngine->DestroyNamedNetDriver(UnitWorld, InDriver->NetDriverName);
	}
}

bool UMinimalClient::NotifyAcceptingChannel(UChannel* Channel)
{
	bool bAccepted = true;

	if (Channel->ChName == NAME_Actor)
	{
	}

	return bAccepted;
}
