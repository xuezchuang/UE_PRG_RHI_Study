#include "NvidiaHBAOPlusEditorCommands.h"

#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "NvidiaHBAOPlusEditorCommands"

FNvidiaHBAOPlusEditorCommands::FNvidiaHBAOPlusEditorCommands()
	: TCommands<FNvidiaHBAOPlusEditorCommands>(
		TEXT("NvidiaHBAOPlusEditor"),
		LOCTEXT("ContextDescription", "NVIDIA HBAO+ Editor"),
		NAME_None,
		FAppStyle::GetAppStyleSetName())
{
}

void FNvidiaHBAOPlusEditorCommands::RegisterCommands()
{
	UI_COMMAND(
		ToggleEnabled,
		"Enable HBAO+",
		"Enable or disable HBAO+ rendering",
		EUserInterfaceActionType::ToggleButton,
		FInputChord());

	UI_COMMAND(
		ToggleVisualizeAO,
		"Visualize AO",
		"Show the raw HBAO+ result instead of multiplying scene color",
		EUserInterfaceActionType::ToggleButton,
		FInputChord());

	UI_COMMAND(
		ResetContext,
		"Reset Context",
		"Wait for the GPU and recreate the native HBAO+ context",
		EUserInterfaceActionType::Button,
		FInputChord());
}

#undef LOCTEXT_NAMESPACE
