// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPViewportCommands.h"

#define LOCTEXT_NAMESPACE "FTPViewportModule"

void FTPViewportCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "TPViewport", "Bring up TPViewport window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
