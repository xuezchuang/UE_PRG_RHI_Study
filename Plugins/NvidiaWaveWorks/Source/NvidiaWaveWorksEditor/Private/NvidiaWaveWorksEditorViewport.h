#pragma once

#include "EditorViewportClient.h"
#include "SAssetEditorViewport.h"
#include "SCommonEditorViewportToolbarBase.h"

class ANvidiaWaveWorksPreviewActor;
class FAdvancedPreviewScene;
class FNvidiaWaveWorksAssetEditorToolkit;

class SNvidiaWaveWorksEditorViewport final
	: public SAssetEditorViewport
	, public ICommonEditorViewportToolbarInfoProvider
	, public FGCObject
{
public:
	SLATE_BEGIN_ARGS(SNvidiaWaveWorksEditorViewport)
	{
	}
		SLATE_ARGUMENT(
			TWeakPtr<FNvidiaWaveWorksAssetEditorToolkit>,
			WaveWorksEditorToolkit)
	SLATE_END_ARGS()

	SNvidiaWaveWorksEditorViewport();
	virtual ~SNvidiaWaveWorksEditorViewport() override;

	void Construct(const FArguments& InArgs);

	virtual TSharedRef<SEditorViewport> GetViewportWidget() override;
	virtual TSharedPtr<FExtender> GetExtenders() const override;
	virtual void OnFloatingButtonClicked() override;

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;

	void SetSimulationPaused(bool bPaused);
	void ResetSimulation();

protected:
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	// 5.2 基类钩子叫 MakeViewportToolbar（BuildViewportToolbar 是新版名字）。
	virtual TSharedPtr<SWidget> MakeViewportToolbar() override;
	virtual void PopulateViewportOverlays(TSharedRef<SOverlay> Overlay) override;

private:
	TSharedPtr<FAdvancedPreviewScene> PreviewScene;
	TSharedPtr<FEditorViewportClient> EditorViewportClient;
	TWeakPtr<FNvidiaWaveWorksAssetEditorToolkit> ToolkitWeak;
	TObjectPtr<ANvidiaWaveWorksPreviewActor> PreviewActor = nullptr;
};

class FNvidiaWaveWorksEditorViewportClient final : public FEditorViewportClient
{
public:
	FNvidiaWaveWorksEditorViewportClient(
		FPreviewScene* InPreviewScene,
		const TWeakPtr<SEditorViewport>& InViewportWidget);

	virtual void Tick(float DeltaSeconds) override;
};
