// Copyright Epic Games, Inc. All Rights Reserved.

#include "DialogueUICommands.h"

#define LOCTEXT_NAMESPACE "DialogueUICommands"

void FDialogueUICommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Dialogue Graph", "Dialogue Builder Graph", EUserInterfaceActionType::ToggleButton, FInputChord());
}

#undef LOCTEXT_NAMESPACE
