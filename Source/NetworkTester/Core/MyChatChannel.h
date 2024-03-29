// Copyright Epic Games, Inc. All Rights Reserved.
// 
//

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/Channel.h"
#include "MyChatChannel.generated.h"


class FInBunch;
class UNetConnection;
class UMinimalClient;


/*
 * A net channel for overriding the implementation of traditional net channels
 */
UCLASS(transient)
class UMyChatChannel : public UChannel
{
	GENERATED_UCLASS_BODY()

	virtual void Init(UNetConnection* InConnection, int32 InChIndex, EChannelCreateFlags CreateFlags) override;

	virtual void ReceivedBunch(FInBunch& Bunch) override;

	virtual void Tick() override;

public:
	/** Whether or not this channel should verify it has been opened (resends initial packets until acked, like control channel) */
	bool bVerifyOpen;
};
