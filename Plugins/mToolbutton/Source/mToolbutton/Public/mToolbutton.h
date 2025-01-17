// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FmToolbuttonModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();

	void ToolButtonClicked(FToolBarBuilder& ToolbarBuilder);

	void OnEngineInitialized();

private:

	void RegisterMenus();

	//UFUNCTION()
	void OnAssetEditorOpened(UObject* Asset, IAssetEditorInstance* EditorInstance);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
