// Copyright 2022 Greg Shynkar. All Rights Reserved

#include "DialogueBuilderEditorModule.h"
#include "ToolMenus/Public/ToolMenuEntry.h"
#include "DialogueUICommands.h"
//#include "ToolMenus/Public/mToolbuttonCommands.h"

#define LOCTEXT_NAMESPACE "DialogueBuilderCustomEditor"



void FDialogueBuilderEditorModule::StartupModule()
{	
	//Registered AssetCategory;
	IAssetTools& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	EAssetTypeCategories::Type DialogueBuilderCategoryType = AssetToolsModule.RegisterAdvancedAssetCategory(TEXT("DialogueBuilder"), LOCTEXT("DialogueBuilderCategory", "DialogueBuilder"));
	TSharedRef<IAssetTypeActions> DialogueBuilderActions = MakeShareable(new FDialogueBuilderActions(DialogueBuilderCategoryType));
	AssetToolsModule.RegisterAssetTypeActions(DialogueBuilderActions);
	
	
	//Registered Visual Parts: Dialogue Nodes, Pin and PinConnectionFactory;
	FEdGraphUtilities::RegisterVisualNodeFactory(MakeShareable(new FDialogueBuilderGraphNodeFactory()));
	FEdGraphUtilities::RegisterVisualPinConnectionFactory(MakeShareable(new FDialogueBuilderGraphPinConnectionFactory()));

	//FmToolbuttonStyle::Initialize();
	//FmToolbuttonStyle::ReloadTextures();
	//FPluginCommands::Register();
	//PluginCommands = MakeShareable(new FUICommandList);
	//PluginCommands->MapAction(
	//	FPluginCommands::Get().OpenPluginWindow,
	//	FExecuteAction::CreateRaw(this, &FDialogueBuilderEditorModule::PluginButtonClicked),
	//	FCanExecuteAction());
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FDialogueBuilderEditorModule::RegisterMenus));
}

void FDialogueBuilderEditorModule::ShutdownModule()
{
	// Unregister properties when the module is shutdown;
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.NotifyCustomizationModuleChanged();
	}

	//Unregister Visual Parts: Dialogue Nodes, Pin and PinConnectionFactory;
	FEdGraphUtilities::UnregisterVisualNodeFactory(MakeShareable(new FDialogueBuilderGraphNodeFactory()));
	FEdGraphUtilities::UnregisterVisualPinConnectionFactory(MakeShareable(new FDialogueBuilderGraphPinConnectionFactory()));
}


void FDialogueBuilderEditorModule::RegisterMenus()
{
	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("AssetEditor.DialogueBuilderEditor.ToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FDialogueUICommands::Register();
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FDialogueUICommands::Get().PluginAction));
			}
		}
	}

	//{
	//	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("AssetEditor.BlueprintEditor.ToolBar");
	//	{
	//		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
	//		{
	//			FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FPluginCommands::Get().OpenPluginWindow));
	//			Entry.SetCommandList(PluginCommands);
	//		}
	//	}
	//}
}


void FDialogueBuilderEditorModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
		LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
		FText::FromString(TEXT("FmToolbuttonModule::PluginButtonClicked()")),
		FText::FromString(TEXT("mToolbutton.cpp"))
	);
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDialogueBuilderEditorModule, DialogueBuilderEditor)