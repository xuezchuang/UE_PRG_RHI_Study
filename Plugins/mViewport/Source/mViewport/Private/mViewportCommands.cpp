// Copyright Epic Games, Inc. All Rights Reserved.

#include "mViewportCommands.h"

#define LOCTEXT_NAMESPACE "FmViewportModule"

void FmViewportCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "mViewport", "Bring up mViewport window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
