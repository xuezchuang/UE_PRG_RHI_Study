// Copyright Epic Games, Inc. All Rights Reserved.

#include "mToolbuttonCommands.h"

#define LOCTEXT_NAMESPACE "FmToolbuttonModule"

void FmToolbuttonCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "mToolbutton", "Execute mToolbutton action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
