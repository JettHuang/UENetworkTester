// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "NetworkTesterStyle.h"

class FNetworkTesterCommands : public TCommands<FNetworkTesterCommands>
{
public:

	FNetworkTesterCommands()
		: TCommands<FNetworkTesterCommands>(TEXT("NetworkTester"), NSLOCTEXT("Contexts", "NetworkTester", "NetworkTester Plugin"), NAME_None, FNetworkTesterStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};