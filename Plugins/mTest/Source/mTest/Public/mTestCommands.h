// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "mTestStyle.h"

class FmTestCommands : public TCommands<FmTestCommands>
{
public:

	FmTestCommands()
		: TCommands<FmTestCommands>(TEXT("mTest"), NSLOCTEXT("Contexts", "mTest", "mTest Plugin"), NAME_None, FmTestStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
