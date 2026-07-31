#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FNvidiaHBAOPlusEditorCommands final
	: public TCommands<FNvidiaHBAOPlusEditorCommands>
{
public:
	FNvidiaHBAOPlusEditorCommands();

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> ToggleEnabled;
	TSharedPtr<FUICommandInfo> ToggleVisualizeAO;
	TSharedPtr<FUICommandInfo> ResetContext;
};
