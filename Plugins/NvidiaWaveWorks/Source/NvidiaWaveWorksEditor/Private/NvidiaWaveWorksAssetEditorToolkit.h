#pragma once

#include "Misc/NotifyHook.h"
#include "Toolkits/AssetEditorToolkit.h"

class FEditorViewportTabContent;
class IDetailsView;
class IToolkitHost;
class UNvidiaWaveWorksAsset;

class FNvidiaWaveWorksAssetEditorToolkit final
	: public FAssetEditorToolkit
	, public FNotifyHook
	, public FGCObject
{
public:
	void InitWaveWorksEditor(
		EToolkitMode::Type Mode,
		const TSharedPtr<IToolkitHost>& InitToolkitHost,
		UNvidiaWaveWorksAsset* AssetToEdit);

	UNvidiaWaveWorksAsset* GetEditedAsset() const
	{
		return EditedAsset;
	}

	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;

private:
	static const FName ViewportTabId;
	static const FName DetailsTabId;

	virtual void RegisterTabSpawners(
		const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(
		const TSharedRef<FTabManager>& InTabManager) override;

	TSharedRef<SDockTab> SpawnViewportTab(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnDetailsTab(const FSpawnTabArgs& Args);

	void BindCommands();
	void ExtendToolbar();
	void FillToolbar(
		FToolBarBuilder& ToolbarBuilder,
		const TSharedRef<FUICommandList> CommandList);
	void TogglePauseSimulation();
	void ResetSimulation();
	bool IsSimulationPaused() const;

	TObjectPtr<UNvidiaWaveWorksAsset> EditedAsset = nullptr;
	TSharedPtr<IDetailsView> DetailsView;
	TSharedPtr<FEditorViewportTabContent> ViewportTabContent;
	bool bSimulationPaused = false;
};
