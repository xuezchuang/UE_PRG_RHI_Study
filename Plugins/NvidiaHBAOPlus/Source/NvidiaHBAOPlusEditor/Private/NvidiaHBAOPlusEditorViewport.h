#pragma once

#include "EditorViewportClient.h"
#include "SAssetEditorViewport.h"
#include "SCommonEditorViewportToolbarBase.h"

class AActor;
class FAdvancedPreviewScene;
class FNvidiaHBAOPlusAssetEditorToolkit;

class SNvidiaHBAOPlusEditorViewport final
	: public SAssetEditorViewport
	, public ICommonEditorViewportToolbarInfoProvider
	, public FGCObject
{
public:
	SLATE_BEGIN_ARGS(SNvidiaHBAOPlusEditorViewport)
	{
	}
		SLATE_ARGUMENT(
			TWeakPtr<FNvidiaHBAOPlusAssetEditorToolkit>,
			HBAOPlusEditorToolkit)
	SLATE_END_ARGS()

	SNvidiaHBAOPlusEditorViewport();
	virtual ~SNvidiaHBAOPlusEditorViewport() override;

	void Construct(const FArguments& InArgs);

	virtual TSharedRef<SEditorViewport> GetViewportWidget() override;
	virtual TSharedPtr<FExtender> GetExtenders() const override;
	virtual void OnFloatingButtonClicked() override;

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;

protected:
	virtual TSharedRef<FEditorViewportClient>
		MakeEditorViewportClient() override;
	// 5.2 基类钩子叫 MakeViewportToolbar（BuildViewportToolbar 是新版名字）。
	virtual TSharedPtr<SWidget> MakeViewportToolbar() override;
	virtual void PopulateViewportOverlays(
		TSharedRef<SOverlay> Overlay) override;

private:
	void BuildPreviewScene();
	AActor* SpawnPreviewMesh(
		const TCHAR* MeshPath,
		const FTransform& Transform);

	TSharedPtr<FAdvancedPreviewScene> PreviewScene;
	TSharedPtr<FEditorViewportClient> EditorViewportClient;
	TWeakPtr<FNvidiaHBAOPlusAssetEditorToolkit> ToolkitWeak;
	TArray<TObjectPtr<AActor>> PreviewActors;
};

class FNvidiaHBAOPlusEditorViewportClient final
	: public FEditorViewportClient
{
public:
	FNvidiaHBAOPlusEditorViewportClient(
		FPreviewScene* InPreviewScene,
		const TWeakPtr<SEditorViewport>& InViewportWidget);

	virtual void Tick(float DeltaSeconds) override;
};
