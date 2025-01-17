// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "mViewportStyle.h"

class FmViewportCommands : public TCommands<FmViewportCommands>
{
public:

	FmViewportCommands()
		: TCommands<FmViewportCommands>(TEXT("mViewport"), NSLOCTEXT("Contexts", "mViewport", "mViewport Plugin"), NAME_None, FmViewportStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};