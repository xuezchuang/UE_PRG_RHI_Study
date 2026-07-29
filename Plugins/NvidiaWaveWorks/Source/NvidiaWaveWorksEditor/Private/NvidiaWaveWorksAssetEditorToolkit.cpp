#include "NvidiaWaveWorksAssetEditorToolkit.h"

#include "DetailsViewArgs.h"
#include "EditorViewportTabContent.h"
#include "Framework/Commands/UICommandList.h"
#include "IDetailsView.h"
#include "Modules/ModuleManager.h"
#include "NvidiaWaveWorksAsset.h"
#include "NvidiaWaveWorksEditorCommands.h"
#include "NvidiaWaveWorksEditorViewport.h"
#include "PropertyEditorModule.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "NvidiaWaveWorksAssetEditorToolkit"

const FName FNvidiaWaveWorksAssetEditorToolkit::ViewportTabId(
	TEXT("NvidiaWaveWorksEditor_Viewport"));
const FName FNvidiaWaveWorksAssetEditorToolkit::DetailsTabId(
	TEXT("NvidiaWaveWorksEditor_Details"));

void FNvidiaWaveWorksAssetEditorToolkit::InitWaveWorksEditor(
	EToolkitMode::Type Mode,
	const TSharedPtr<IToolkitHost>& InitToolkitHost,
	UNvidiaWaveWorksAsset* AssetToEdit)
{
	EditedAsset = AssetToEdit;
	check(EditedAsset);

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
		FTabManager::NewLayout(TEXT("Standalone_NvidiaWaveWorksEditor_Layout_v1"))
			->AddArea(
				FTabManager::NewPrimaryArea()
					->SetOrientation(Orient_Vertical)
					->Split(
						FTabManager::NewSplitter()
							->SetOrientation(Orient_Horizontal)
							->Split(
								FTabManager::NewStack()
									->SetSizeCoefficient(0.68f)
									->AddTab(ViewportTabId, ETabState::OpenedTab)
									->SetHideTabWell(true))
							->Split(
								FTabManager::NewStack()
									->SetSizeCoefficient(0.32f)
									->AddTab(DetailsTabId, ETabState::OpenedTab))));

	InitAssetEditor(
		Mode,
		InitToolkitHost,
		TEXT("NvidiaWaveWorksEditorApp"),
		Layout,
		true,
		true,
		EditedAsset);

	ExtendToolbar();
	RegenerateMenusAndToolbars();
}

void FNvidiaWaveWorksAssetEditorToolkit::RegisterTabSpawners(
	const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory =
		InTabManager->AddLocalWorkspaceMenuCategory(
			LOCTEXT("WorkspaceMenu", "NVIDIA WaveWorks"));

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager
		->RegisterTabSpawner(
			ViewportTabId,
			FOnSpawnTab::CreateSP(
				this,
				&FNvidiaWaveWorksAssetEditorToolkit::SpawnViewportTab))
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
				&FNvidiaWaveWorksAssetEditorToolkit::SpawnDetailsTab))
		.SetDisplayName(LOCTEXT("DetailsTab", "Details"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
		.SetIcon(FSlateIcon(
			FAppStyle::GetAppStyleSetName(),
			TEXT("LevelEditor.Tabs.Details")));
}

void FNvidiaWaveWorksAssetEditorToolkit::UnregisterTabSpawners(
	const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	InTabManager->UnregisterTabSpawner(ViewportTabId);
	InTabManager->UnregisterTabSpawner(DetailsTabId);
}

TSharedRef<SDockTab> FNvidiaWaveWorksAssetEditorToolkit::SpawnViewportTab(
	const FSpawnTabArgs& Args)
{
	TSharedRef<SDockTab> DockTab = SNew(SDockTab);
	ViewportTabContent = MakeShared<FEditorViewportTabContent>();

	const TWeakPtr<FNvidiaWaveWorksAssetEditorToolkit> WeakToolkit =
		SharedThis(this);
	ViewportTabContent->Initialize(
		[WeakToolkit](const FAssetEditorViewportConstructionArgs&)
		{
			return SNew(SNvidiaWaveWorksEditorViewport)
				.WaveWorksEditorToolkit(WeakToolkit);
		},
		DockTab,
		TEXT("NvidiaWaveWorksEditorViewport"));

	return DockTab;
}

TSharedRef<SDockTab> FNvidiaWaveWorksAssetEditorToolkit::SpawnDetailsTab(
	const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == DetailsTabId);
	return SNew(SDockTab)
		.Label(LOCTEXT("DetailsTabTitle", "Details"))
		[
			DetailsView.ToSharedRef()
		];
}

void FNvidiaWaveWorksAssetEditorToolkit::BindCommands()
{
	const FNvidiaWaveWorksEditorCommands& Commands =
		FNvidiaWaveWorksEditorCommands::Get();

	GetToolkitCommands()->MapAction(
		Commands.TogglePauseSimulation,
		FExecuteAction::CreateSP(
			this,
			&FNvidiaWaveWorksAssetEditorToolkit::TogglePauseSimulation),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(
			this,
			&FNvidiaWaveWorksAssetEditorToolkit::IsSimulationPaused));

	GetToolkitCommands()->MapAction(
		Commands.ResetSimulation,
		FExecuteAction::CreateSP(
			this,
			&FNvidiaWaveWorksAssetEditorToolkit::ResetSimulation));
}

void FNvidiaWaveWorksAssetEditorToolkit::ExtendToolbar()
{
	TSharedPtr<FExtender> ToolbarExtender = MakeShared<FExtender>();
	ToolbarExtender->AddToolBarExtension(
		TEXT("Asset"),
		EExtensionHook::After,
		GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(
			this,
			&FNvidiaWaveWorksAssetEditorToolkit::FillToolbar,
			GetToolkitCommands()));
	AddToolbarExtender(ToolbarExtender);
}

void FNvidiaWaveWorksAssetEditorToolkit::FillToolbar(
	FToolBarBuilder& ToolbarBuilder,
	const TSharedRef<FUICommandList> CommandList)
{
	ToolbarBuilder.BeginSection(TEXT("WaveWorks"));
	ToolbarBuilder.AddToolBarButton(
		FNvidiaWaveWorksEditorCommands::Get().TogglePauseSimulation);
	ToolbarBuilder.AddToolBarButton(
		FNvidiaWaveWorksEditorCommands::Get().ResetSimulation);
	ToolbarBuilder.EndSection();
}

void FNvidiaWaveWorksAssetEditorToolkit::TogglePauseSimulation()
{
	bSimulationPaused = !bSimulationPaused;
	if (!ViewportTabContent)
	{
		return;
	}

	TFunction<void(FName, TSharedPtr<IEditorViewportLayoutEntity>)> PauseAction =
		[this](FName, TSharedPtr<IEditorViewportLayoutEntity> Entity)
		{
			const TSharedRef<SNvidiaWaveWorksEditorViewport> Viewport =
				StaticCastSharedRef<SNvidiaWaveWorksEditorViewport>(
					Entity->AsWidget());
			Viewport->SetSimulationPaused(bSimulationPaused);
		};
	ViewportTabContent->PerformActionOnViewports(PauseAction);
}

void FNvidiaWaveWorksAssetEditorToolkit::ResetSimulation()
{
	if (!ViewportTabContent)
	{
		return;
	}

	TFunction<void(FName, TSharedPtr<IEditorViewportLayoutEntity>)> ResetAction =
		[](FName, TSharedPtr<IEditorViewportLayoutEntity> Entity)
		{
			const TSharedRef<SNvidiaWaveWorksEditorViewport> Viewport =
				StaticCastSharedRef<SNvidiaWaveWorksEditorViewport>(
					Entity->AsWidget());
			Viewport->ResetSimulation();
		};
	ViewportTabContent->PerformActionOnViewports(ResetAction);
}

bool FNvidiaWaveWorksAssetEditorToolkit::IsSimulationPaused() const
{
	return bSimulationPaused;
}

FName FNvidiaWaveWorksAssetEditorToolkit::GetToolkitFName() const
{
	return TEXT("NvidiaWaveWorksEditor");
}

FText FNvidiaWaveWorksAssetEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "NVIDIA WaveWorks Editor");
}

FString FNvidiaWaveWorksAssetEditorToolkit::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricPrefix", "WaveWorks ").ToString();
}

FLinearColor
FNvidiaWaveWorksAssetEditorToolkit::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.0f, 0.35f, 0.65f, 0.5f);
}

void FNvidiaWaveWorksAssetEditorToolkit::AddReferencedObjects(
	FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EditedAsset);
}

FString FNvidiaWaveWorksAssetEditorToolkit::GetReferencerName() const
{
	return TEXT("FNvidiaWaveWorksAssetEditorToolkit");
}

#undef LOCTEXT_NAMESPACE
