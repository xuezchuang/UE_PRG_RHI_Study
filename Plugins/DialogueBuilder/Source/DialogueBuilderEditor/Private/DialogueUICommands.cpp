// Copyright Epic Games, Inc. All Rights Reserved.

#include "DialogueUICommands.h"

#define LOCTEXT_NAMESPACE "DialogueUICommands"

void FDialogueUICommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Dialogue Graph", "Dialogue Builder Graph", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(PluginAction2, "Dialogue Graph2", "Dialogue Builder Graph2", EUserInterfaceActionType::ToggleButton, FInputChord());
}

#undef LOCTEXT_NAMESPACE
