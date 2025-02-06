// Copyright Epic Games, Inc. All Rights Reserved.

#include "mToolbutton.h"
#include "mToolbuttonStyle.h"
#include "mToolbuttonCommands.h"
#include "Misc/MessageDialog.h"
//#include "Toolkits/AssetEditorManager.h"
#include "ToolMenus.h"
#include "AssetEditorModeManager.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Kismet/Public/BlueprintEditor.h"
#include "ToolMenus/Public/ToolMenuContext.h"
#include "Kismet/Public/BlueprintEditorModule.h"
#include "Kismet/Public/SBlueprintEditorToolbar.h"
#include "TestBuilderObject.h"
#include <Framework/MultiBox/MultiBoxBuilder.h>

static const FName mToolbuttonTabName("mToolbutton");

#define LOCTEXT_NAMESPACE "FmToolbuttonModule"

void FmToolbuttonModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FmToolbuttonStyle::Initialize();
	FmToolbuttonStyle::ReloadTextures();

	FmToolbuttonCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FmToolbuttonCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FmToolbuttonModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FmToolbuttonModule::RegisterMenus));
	//FCoreDelegates::OnPostEngineInit.AddRaw(this, &FmToolbuttonModule::OnEngineInitialized);

	//FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("BlueprintGraph");

	//// 监听蓝图编辑器打开的事件
	//BlueprintEditorModule.OnBlueprintEditorOpened().AddRaw(this, &FmToolbuttonModule::OnAssetEditorOpened);
													 //{
														// //if(FBlueprintEditor* BlueprintEditor = static_cast<FBlueprintEditor*>(EditorInstance))
														// //{


														//	// //// 获取蓝图工具栏并添加按钮
														//	// //BlueprintEditor->AddToolbarExtension(
														//	//	// "ToolBar",
														//	//	// EExtensionHook::After,
														//	//	// PluginCommands,
														//	//	// FToolBarExtensionDelegate::CreateRaw(this, &FmToolbuttonModule::PluginButtonClicked)
														//	// //);
														// //}
													 //});

	// 将按钮添加到蓝图编辑器的工具栏
	//FBlueprintEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("LevelEditor");
	//LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolBarExtender);

}

void FmToolbuttonModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FmToolbuttonStyle::Shutdown();

	FmToolbuttonCommands::Unregister();
}

void FmToolbuttonModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FmToolbuttonModule::PluginButtonClicked()")),
							FText::FromString(TEXT("mToolbutton.cpp"))
					   );
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

void FmToolbuttonModule::ToolButtonClicked(FToolBarBuilder& ToolbarBuilder)
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
		LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
		FText::FromString(TEXT("FmToolbuttonModule::ToolButtonClicked()")),
		FText::FromString(TEXT("mToolbutton.cpp"))
	);
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

void FmToolbuttonModule::OnEngineInitialized()
{
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	AssetEditorSubsystem->OnAssetOpenedInEditor().AddRaw(this, &FmToolbuttonModule::OnAssetEditorOpened);
}

void FmToolbuttonModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FmToolbuttonCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FmToolbuttonCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}

	//{
	//	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("AssetEditor.BlueprintEditor.ToolBar");
	//	{
	//		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
	//		{
	//			FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FmToolbuttonCommands::Get().PluginAction));
	//			Entry.SetCommandList(PluginCommands);
	//		}
	//	}
	//}

}

void FmToolbuttonModule::OnAssetEditorOpened(UObject* Asset, IAssetEditorInstance* EditorInstance)
{
	//ILevelSequenceEditorToolkit* LevelSequenceEditor = static_cast<ILevelSequenceEditorToolkit*>(AssetEditor);
	UBlueprint* BlueprintAsset = Cast<UBlueprint>(Asset);
	if(BlueprintAsset == NULL)
		return;
	//if (const TSharedPtr<FBlueprintEditor> BlueprintEditor = EditorInstance->Pin())
	if(FBlueprintEditor* BlueprintEditor = static_cast<FBlueprintEditor*>(EditorInstance))
	{
		if(!BlueprintAsset->GeneratedClass->IsChildOf(UTestBuilderObject::StaticClass()))
			return;
		if(0)
		{
			FToolMenuOwnerScoped OwnerScoped(this);
			TSharedPtr<FExtender> Extender = MakeShareable(new FExtender());
			//// 向扩展器中添加工具栏按钮
			Extender->AddToolBarExtension(
				//"ToolBar",  // 将扩展添加到工具栏
				"Settings",
				EExtensionHook::After,  // 按钮添加到工具栏后面
				PluginCommands,  // 命令列表
				FToolBarExtensionDelegate::CreateRaw(this, &FmToolbuttonModule::ToolButtonClicked)
			);
			BlueprintEditor->AddToolbarExtender(Extender);
			BlueprintEditor->RegenerateMenusAndToolbars();
		}

		if(0)
		{
			FName OutParentName;
			UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu(BlueprintEditor->GetToolMenuToolbarName(OutParentName));
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FmToolbuttonCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}

		if(0)
		{
			// 获取蓝图编辑器的工具菜单
			UToolMenu* ToolbarMenu = BlueprintEditor->RegisterModeToolbarIfUnregistered(FName(TEXT("mToolbutton")));
			if (ToolbarMenu)
			{
				FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
				{
					FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FmToolbuttonCommands::Get().PluginAction));
					Entry.SetCommandList(PluginCommands);
				}
				BlueprintEditor->GetToolbarBuilder()->AddNewToolbar(ToolbarMenu);
				BlueprintEditor->RegenerateMenusAndToolbars();
			}
		}

		if(0)
		{
			TSharedRef<SButton> ButtonWidget = SNew(SButton)
				.Text(NSLOCTEXT("YourPluginName", "ButtonLabel", "Click Me"))
				.OnClicked(FOnClicked::CreateLambda([]()
					{
						FMessageDialog::Open(EAppMsgType::YesNo, NSLOCTEXT("YourPluginName", "Message", "Button was clicked!"));
						return FReply::Handled();
					}));

			BlueprintEditor->AddToolbarWidget(ButtonWidget);
			BlueprintEditor->GenerateToolbar();
		}

		if (0)
		{
			TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
			ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::Before, nullptr,
				FToolBarExtensionDelegate::CreateRaw(this, &FmToolbuttonModule::ToolButtonClicked));

			BlueprintEditor->AddToolbarExtender(ToolbarExtender);
			// 
			//BlueprintEditor->GetSharedToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
		}

	}
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FmToolbuttonModule, mToolbutton)