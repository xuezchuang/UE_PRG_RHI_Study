// Copyright Epic Games, Inc. All Rights Reserved.

#include "mTestCommands.h"

#define LOCTEXT_NAMESPACE "FmTestModule"

void FmTestCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "mTest", "Execute mTest action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
