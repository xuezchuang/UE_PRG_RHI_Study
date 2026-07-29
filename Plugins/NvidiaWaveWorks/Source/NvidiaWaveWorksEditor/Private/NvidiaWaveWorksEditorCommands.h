#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FNvidiaWaveWorksEditorCommands final
	: public TCommands<FNvidiaWaveWorksEditorCommands>
{
public:
	FNvidiaWaveWorksEditorCommands();

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> TogglePauseSimulation;
	TSharedPtr<FUICommandInfo> ResetSimulation;
};
