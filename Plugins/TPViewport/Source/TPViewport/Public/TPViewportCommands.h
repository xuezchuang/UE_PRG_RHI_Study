// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "TPViewportStyle.h"

class FTPViewportCommands : public TCommands<FTPViewportCommands>
{
public:

	FTPViewportCommands()
		: TCommands<FTPViewportCommands>(TEXT("TPViewport"), NSLOCTEXT("Contexts", "TPViewport", "TPViewport Plugin"), NAME_None, FTPViewportStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};