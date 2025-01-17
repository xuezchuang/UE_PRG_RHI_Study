// Copyright Epic Games, Inc. All Rights Reserved.

#include "mViewport.h"
#include "mViewportStyle.h"
#include "mViewportCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName mViewportTabName("mViewport");

#define LOCTEXT_NAMESPACE "FmViewportModule"

void FmViewportModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FmViewportStyle::Initialize();
	FmViewportStyle::ReloadTextures();

	FmViewportCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FmViewportCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FmViewportModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FmViewportModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(mViewportTabName, FOnSpawnTab::CreateRaw(this, &FmViewportModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FmViewportTabTitle", "mViewport"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FmViewportModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FmViewportStyle::Shutdown();

	FmViewportCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(mViewportTabName);
}

TSharedRef<SDockTab> FmViewportModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FmViewportModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("mViewport.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FmViewportModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(mViewportTabName);
}

void FmViewportModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FmViewportCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FmViewportCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FmViewportModule, mViewport)