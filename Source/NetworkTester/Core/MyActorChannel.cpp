// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyActorChannel.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Engine/NetConnection.h"
#include "GameFramework/PlayerController.h"

#include "MinimalClient.h"


/**
 * Default constructor
 */
UMyActorChannel::UMyActorChannel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MinClient(nullptr)
{
}

void UMyActorChannel::ReceivedBunch(FInBunch& Bunch)
{
	Super::ReceivedBunch(Bunch);
}

void UMyActorChannel::Tick()
{
	Super::Tick();
}

void UMyActorChannel::NotifyActorChannelOpen(AActor* InActor, FInBunch& InBunch)
{
	Super::NotifyActorChannelOpen(InActor, InBunch);
}
