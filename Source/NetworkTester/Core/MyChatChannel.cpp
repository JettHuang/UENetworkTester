// Copyright Epic Games, Inc. All Rights Reserved.
//

#include "MyChatChannel.h"
#include "Engine/NetConnection.h"
#include "MinimalClient.h"


UMyChatChannel::UMyChatChannel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MinClient(nullptr)
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
}

void UMyChatChannel::Tick()
{
	Super::Tick();
}
