// Copyright Epic Games, Inc. All Rights Reserved.
// 
//

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "OnlineSubsystemUtils/Classes/IpConnection.h"
#include "MyConnection.generated.h"


class FInBunch;
class UNetConnection;
class UMinimalClient;


/*
 * A MyConnection
 */
UCLASS(transient)
class UMyConnection : public UIpConnection
{
	GENERATED_UCLASS_BODY()

public:
	/** The minimal client which may require received bunch notifications */
	UMinimalClient* MinClient;

};
