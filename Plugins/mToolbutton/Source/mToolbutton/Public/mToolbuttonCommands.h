// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "mToolbuttonStyle.h"

class FmToolbuttonCommands : public TCommands<FmToolbuttonCommands>
{
public:

	FmToolbuttonCommands()
		: TCommands<FmToolbuttonCommands>(TEXT("mToolbutton"), NSLOCTEXT("Contexts", "mToolbutton", "mToolbutton Plugin"), NAME_None, FmToolbuttonStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
