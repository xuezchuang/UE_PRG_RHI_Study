// Copyright Epic Games, Inc. All Rights Reserved.

#include "mTest.h"
#include "mTestStyle.h"
#include "mTestCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

#include "ShaderCore.h"
#include "Interfaces/IPluginManager.h"

static const FName mTestTabName("mTest");

#define LOCTEXT_NAMESPACE "FmTestModule"

void FmTestModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("mTest"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/mTest"), PluginShaderDir);

	FmTestStyle::Initialize();
	FmTestStyle::ReloadTextures();

	FmTestCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FmTestCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FmTestModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FmTestModule::RegisterMenus));
}

void FmTestModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FmTestStyle::Shutdown();

	FmTestCommands::Unregister();
}

void FmTestModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FmTestModule::PluginButtonClicked()")),
							FText::FromString(TEXT("mTest.cpp"))
					   );
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

void FmTestModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);
	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FmTestCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FmTestCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}

	//{
	//	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("BlueprintEditor.MainMenu.Window");
	//	{
	//		FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
	//		Section.AddMenuEntryWithCommandList(FmTestCommands::Get().PluginAction, PluginCommands);
	//	}
	//}

	//{
	//	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("BlueprintEditor.LevelEditorToolBar.PlayToolBar");
	//	{
	//		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
	//		{
	//			FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FmTestCommands::Get().PluginAction));
	//			Entry.SetCommandList(PluginCommands);
	//		}
	//	}
	//}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FmTestModule, mTest)