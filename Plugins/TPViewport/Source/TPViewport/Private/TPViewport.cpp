// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPViewport.h"
#include "TPViewportStyle.h"
#include "TPViewportCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

#include "Widgets/SViewport.h"
#include "Slate/SceneViewport.h"
#include "RenderTestViewportClient.h"

static const FName TPViewportTabName("TPViewport");

#define LOCTEXT_NAMESPACE "FTPViewportModule"

void FTPViewportModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FTPViewportStyle::Initialize();
	FTPViewportStyle::ReloadTextures();

	FTPViewportCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FTPViewportCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FTPViewportModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FTPViewportModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TPViewportTabName, FOnSpawnTab::CreateRaw(this, &FTPViewportModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FTPViewportTabTitle", "TPViewport"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FTPViewportModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FTPViewportStyle::Shutdown();

	FTPViewportCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TPViewportTabName);
}

TSharedRef<SDockTab> FTPViewportModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	TSharedRef<SDockTab> Tab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SBox)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SAssignNew(ViewportWidget, SViewport)
			]
		];

	ViewportClient = MakeShareable(new FRenderTestViewportClient());
	Viewport = MakeShareable(new FSceneViewport(ViewportClient.Get(), ViewportWidget));
	ViewportWidget->SetViewportInterface(Viewport.ToSharedRef());

	return Tab;
}

void FTPViewportModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(TPViewportTabName);
}

void FTPViewportModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FTPViewportCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FTPViewportCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTPViewportModule, TPViewport)