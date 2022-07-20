// Copyright Epic Games, Inc. All Rights Reserved.
//

#include "MyConnection.h"
#include "Engine/NetConnection.h"
#include "MinimalClient.h"


UMyConnection::UMyConnection(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MinClient(nullptr)
{
}

