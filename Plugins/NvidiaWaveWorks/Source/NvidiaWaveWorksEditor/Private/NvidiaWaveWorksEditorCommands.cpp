#include "NvidiaWaveWorksEditorCommands.h"

#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "NvidiaWaveWorksEditorCommands"

FNvidiaWaveWorksEditorCommands::FNvidiaWaveWorksEditorCommands()
	: TCommands<FNvidiaWaveWorksEditorCommands>(
		TEXT("NvidiaWaveWorksEditor"),
		LOCTEXT("ContextDescription", "NVIDIA WaveWorks Editor"),
		NAME_None,
		FAppStyle::GetAppStyleSetName())
{
}

void FNvidiaWaveWorksEditorCommands::RegisterCommands()
{
	UI_COMMAND(
		TogglePauseSimulation,
		"Pause Simulation",
		"Pause or resume WaveWorks time",
		EUserInterfaceActionType::ToggleButton,
		FInputChord());

	UI_COMMAND(
		ResetSimulation,
		"Reset Simulation",
		"Reset WaveWorks time and rebuild the simulation",
		EUserInterfaceActionType::Button,
		FInputChord());
}

#undef LOCTEXT_NAMESPACE
