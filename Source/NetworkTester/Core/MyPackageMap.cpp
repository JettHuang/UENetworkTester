// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyPackageMap.h"
#include "GameFramework/Actor.h"

#include "MinimalClient.h"


UMyPackageMapClient::UMyPackageMapClient(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bWithinSerializeNewActor(false)
	, bPendingArchetypeSpawn(false)
	, ReplaceObjects()
	, OnSerializeName()
{
}

bool UMyPackageMapClient::SerializeObject(FArchive& Ar, UClass* InClass, UObject*& Obj, FNetworkGUID* OutNetGUID)
{
	return Super::SerializeObject(Ar, InClass, Obj, OutNetGUID);
}

bool UMyPackageMapClient::SerializeName(FArchive& Ar, FName& InName)
{
	return Super::SerializeName(Ar, InName);
}

bool UMyPackageMapClient::SerializeNewActor(FArchive& Ar, class UActorChannel* Channel, class AActor*& Actor)
{
	return Super::SerializeNewActor(Ar, Channel, Actor);
}

