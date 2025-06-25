// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FDialogueUICommands : public TCommands<FDialogueUICommands>
{
public:

	FDialogueUICommands()
		: TCommands<FDialogueUICommands>(TEXT("DialogueUICommands"), NSLOCTEXT("Contexts", "DialogueUICommands", "Plugin Commands"), NAME_None, TEXT("EditorStyle"))
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
	TSharedPtr< FUICommandInfo > PluginAction2;
};
