// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetworkTesterCommands.h"

#define LOCTEXT_NAMESPACE "FNetworkTesterModule"

void FNetworkTesterCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "NetworkTester", "Bring up NetworkTester window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
