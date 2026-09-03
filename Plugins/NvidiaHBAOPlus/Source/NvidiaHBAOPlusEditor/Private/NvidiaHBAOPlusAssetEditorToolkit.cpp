#include "NvidiaHBAOPlusAssetEditorToolkit.h"

#include "DetailsViewArgs.h"
#include "EditorViewportTabContent.h"
#include "Framework/Commands/UICommandList.h"
#include "IDetailsView.h"
#include "INvidiaHBAOPlusModule.h"
#include "Modules/ModuleManager.h"
#include "NvidiaHBAOPlusAsset.h"
#include "NvidiaHBAOPlusEditorCommands.h"
#include "NvidiaHBAOPlusEditorViewport.h"
#include "PropertyEditorModule.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "NvidiaHBAOPlusAssetEditorToolkit"

const FName FNvidiaHBAOPlusAssetEditorToolkit::ViewportTabId(
	TEXT("NvidiaHBAOPlusEditor_Viewport"));
const FName FNvidiaHBAOPlusAssetEditorToolkit::DetailsTabId(
	TEXT("NvidiaHBAOPlusEditor_Details"));

FNvidiaHBAOPlusAssetEditorToolkit::~FNvidiaHBAOPlusAssetEditorToolkit()
{
	if (EditedAsset)
	{
		EditedAsset->OnSettingsChanged().RemoveAll(this);
	}
}

void FNvidiaHBAOPlusAssetEditorToolkit::InitHBAOPlusEditor(
	EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost,
	UNvidiaHBAOPlusAsset* AssetToEdit)
{
	EditedAsset = AssetToEdit;
	check(EditedAsset);

	EditedAsset->OnSettingsChanged().AddSP(
		this,
		&FNvidiaHBAOPlusAssetEditorToolkit::HandleSettingsChanged);
	HandleSettingsChanged();
	BindCommands();

	FPropertyEditorModule& PropertyEditorModule =
		FModuleManager::LoadModuleChecked<FPropertyEditorModule>(
			TEXT("PropertyEditor"));

	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bAllowSearch = true;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.bUpdatesFromSelection = false;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.NotifyHook = this;

	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObject(EditedAsset);

	const TSharedRef<FTabManager::FLayout> Layout =
		FTabManager::NewLayout(
			TEXT("Standalone_NvidiaHBAOPlusEditor_Layout_v1"))
			->AddArea(
				FTabManager::NewPrimaryArea()
					->SetOrientation(Orient_Vertical)
					->Split(
						FTabManager::NewSplitter()
							->SetOrientation(Orient_Horizontal)
							->Split(
								FTabManager::NewStack()
									->SetSizeCoefficient(0.68f)
									->AddTab(
										ViewportTabId,
										ETabState::OpenedTab)
									->SetHideTabWell(true))
							->Split(
								FTabManager::NewStack()
									->SetSizeCoefficient(0.32f)
									->AddTab(
										DetailsTabId,
										ETabState::OpenedTab))));

	InitAssetEditor(
		Mode,
		InitToolkitHost,
		TEXT("NvidiaHBAOPlusEditorApp"),
		Layout,
		true,
		true,
		EditedAsset);

	ExtendToolbar();
	RegenerateMenusAndToolbars();
}

void FNvidiaHBAOPlusAssetEditorToolkit::RegisterTabSpawners(
	const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory =
		InTabManager->AddLocalWorkspaceMenuCategory(
			LOCTEXT("WorkspaceMenu", "NVIDIA HBAO+"));

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager
		->RegisterTabSpawner(
			ViewportTabId,
			FOnSpawnTab::CreateSP(
				this,
				&FNvidiaHBAOPlusAssetEditorToolkit::SpawnViewportTab))
		.SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
		.SetIcon(FSlateIcon(
			FAppStyle::GetAppStyleSetName(),
			TEXT("LevelEditor.Tabs.Viewports")));

	InTabManager
		->RegisterTabSpawner(
			DetailsTabId,
			FOnSpawnTab::CreateSP(
				this,
				&FNvidiaHBAOPlusAssetEditorToolkit::SpawnDetailsTab))
		.SetDisplayName(LOCTEXT("DetailsTab", "Details"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
		.SetIcon(FSlateIcon(
			FAppStyle::GetAppStyleSetName(),
			TEXT("LevelEditor.Tabs.Details")));
}

void FNvidiaHBAOPlusAssetEditorToolkit::UnregisterTabSpawners(
	const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	InTabManager->UnregisterTabSpawner(ViewportTabId);
	InTabManager->UnregisterTabSpawner(DetailsTabId);
}

TSharedRef<SDockTab>
FNvidiaHBAOPlusAssetEditorToolkit::SpawnViewportTab(
	const FSpawnTabArgs& Args)
{
	TSharedRef<SDockTab> DockTab = SNew(SDockTab);
	ViewportTabContent = MakeShared<FEditorViewportTabContent>();

	const TWeakPtr<FNvidiaHBAOPlusAssetEditorToolkit> WeakToolkit =
		SharedThis(this);
	ViewportTabContent->Initialize(
		[WeakToolkit](const FAssetEditorViewportConstructionArgs&)
		{
			return SNew(SNvidiaHBAOPlusEditorViewport)
				.HBAOPlusEditorToolkit(WeakToolkit);
		},
		DockTab,
		TEXT("NvidiaHBAOPlusEditorViewport"));

	return DockTab;
}

TSharedRef<SDockTab>
FNvidiaHBAOPlusAssetEditorToolkit::SpawnDetailsTab(
	const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == DetailsTabId);
	return SNew(SDockTab)
		.Label(LOCTEXT("DetailsTabTitle", "Details"))
		[
			DetailsView.ToSharedRef()
		];
}

void FNvidiaHBAOPlusAssetEditorToolkit::BindCommands()
{
	const FNvidiaHBAOPlusEditorCommands& Commands =
		FNvidiaHBAOPlusEditorCommands::Get();

	GetToolkitCommands()->MapAction(
		Commands.ToggleEnabled,
		FExecuteAction::CreateSP(
			this,
			&FNvidiaHBAOPlusAssetEditorToolkit::ToggleEnabled),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(
			this,
			&FNvidiaHBAOPlusAssetEditorToolkit::IsEnabled));

	GetToolkitCommands()->MapAction(
		Commands.ToggleVisualizeAO,
		FExecuteAction::CreateSP(
			this,
			&FNvidiaHBAOPlusAssetEditorToolkit::ToggleVisualizeAO),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(
			this,
			&FNvidiaHBAOPlusAssetEditorToolkit::IsVisualizingAO));

	GetToolkitCommands()->MapAction(
		Commands.ResetContext,
		FExecuteAction::CreateSP(
			this,
			&FNvidiaHBAOPlusAssetEditorToolkit::ResetContext));
}

void FNvidiaHBAOPlusAssetEditorToolkit::ExtendToolbar()
{
	TSharedPtr<FExtender> ToolbarExtender = MakeShared<FExtender>();
	ToolbarExtender->AddToolBarExtension(
		TEXT("Asset"),
		EExtensionHook::After,
		GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(
			this,
			&FNvidiaHBAOPlusAssetEditorToolkit::FillToolbar,
			GetToolkitCommands()));
	AddToolbarExtender(ToolbarExtender);
}

void FNvidiaHBAOPlusAssetEditorToolkit::FillToolbar(
	FToolBarBuilder& ToolbarBuilder,
	const TSharedRef<FUICommandList> CommandList)
{
	ToolbarBuilder.BeginSection(TEXT("HBAOPlus"));
	ToolbarBuilder.AddToolBarButton(
		FNvidiaHBAOPlusEditorCommands::Get().ToggleEnabled);
	ToolbarBuilder.AddToolBarButton(
		FNvidiaHBAOPlusEditorCommands::Get().ToggleVisualizeAO);
	ToolbarBuilder.AddToolBarButton(
		FNvidiaHBAOPlusEditorCommands::Get().ResetContext);
	ToolbarBuilder.EndSection();
}

void FNvidiaHBAOPlusAssetEditorToolkit::ToggleEnabled()
{
	if (!EditedAsset)
	{
		return;
	}

	EditedAsset->Modify();
	EditedAsset->bEnabled = !EditedAsset->bEnabled;
	EditedAsset->MarkPackageDirty();
	EditedAsset->NotifySettingsChanged();
}

void FNvidiaHBAOPlusAssetEditorToolkit::ToggleVisualizeAO()
{
	if (!EditedAsset)
	{
		return;
	}

	EditedAsset->Modify();
	EditedAsset->bVisualizeAO = !EditedAsset->bVisualizeAO;
	EditedAsset->MarkPackageDirty();
	EditedAsset->NotifySettingsChanged();
}

void FNvidiaHBAOPlusAssetEditorToolkit::ResetContext()
{
	if (INvidiaHBAOPlusModule::IsAvailable())
	{
		INvidiaHBAOPlusModule::Get().RequestContextReset();
	}
}

bool FNvidiaHBAOPlusAssetEditorToolkit::IsEnabled() const
{
	return EditedAsset && EditedAsset->bEnabled;
}

bool FNvidiaHBAOPlusAssetEditorToolkit::IsVisualizingAO() const
{
	return EditedAsset && EditedAsset->bVisualizeAO;
}

void FNvidiaHBAOPlusAssetEditorToolkit::HandleSettingsChanged()
{
	if (EditedAsset && INvidiaHBAOPlusModule::IsAvailable())
	{
		INvidiaHBAOPlusModule::Get().ApplyAssetSettings(*EditedAsset);
	}
}

FName FNvidiaHBAOPlusAssetEditorToolkit::GetToolkitFName() const
{
	return TEXT("NvidiaHBAOPlusEditor");
}

FText FNvidiaHBAOPlusAssetEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "NVIDIA HBAO+ Editor");
}

FString FNvidiaHBAOPlusAssetEditorToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricPrefix", "HBAO+ ").ToString();
}

FLinearColor
FNvidiaHBAOPlusAssetEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.20f, 0.55f, 0.08f, 0.5f);
}

void FNvidiaHBAOPlusAssetEditorToolkit::AddReferencedObjects(
	FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EditedAsset);
}

FString FNvidiaHBAOPlusAssetEditorToolkit::GetReferencerName() const
{
	return TEXT("FNvidiaHBAOPlusAssetEditorToolkit");
}

#undef LOCTEXT_NAMESPACE
