// Copyright Epic Games, Inc. All Rights Reserved.
//

#include "MyChatChannel.h"
#include "Engine/NetConnection.h"
#include "MyConnection.h"
#include "MinimalClient.h"


UMyChatChannel::UMyChatChannel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bVerifyOpen(false)
{
	ChName = NAME_Voice;
}

void UMyChatChannel::Init(UNetConnection* InConnection, int32 InChIndex, EChannelCreateFlags CreateFlags)
{
	Super::Init(InConnection, InChIndex, CreateFlags);
}

void UMyChatChannel::ReceivedBunch(FInBunch& Bunch)
{
	FString Text;

	Bunch << Text;
	
	UE_LOG(LogNet, Warning, TEXT("UMyChannel::ReceivedBunch: %s\n"), *Text);

	UMyConnection* MyConnection = Cast<UMyConnection>(Connection);
	if (MyConnection && MyConnection->MinClient)
	{
		MyConnection->MinClient->ReceiveMessageDel.Broadcast(Text, Connection);
	}
}

void UMyChatChannel::Tick()
{
	Super::Tick();
}
