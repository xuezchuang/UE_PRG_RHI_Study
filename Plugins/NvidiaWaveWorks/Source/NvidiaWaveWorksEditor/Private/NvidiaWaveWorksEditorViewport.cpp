#include "NvidiaWaveWorksEditorViewport.h"

#include "AdvancedPreviewScene.h"
#include "CoreGlobals.h"
#include "NvidiaWaveWorksAsset.h"
#include "NvidiaWaveWorksAssetEditorToolkit.h"
#include "NvidiaWaveWorksPreviewActor.h"
#include "PreviewProfileController.h"
#include "Styling/AppStyle.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"

namespace NvidiaWaveWorksEditorViewportLocals
{
	class SViewportToolbar final : public SCommonEditorViewportToolbarBase
	{
	public:
		SLATE_BEGIN_ARGS(SViewportToolbar)
		{
		}
		SLATE_END_ARGS()

		void Construct(
			const FArguments& InArgs,
			TSharedPtr<ICommonEditorViewportToolbarInfoProvider> InProvider)
		{
			SCommonEditorViewportToolbarBase::Construct(
				SCommonEditorViewportToolbarBase::FArguments()
					.AddRealtimeButton(false)
					.PreviewProfileController(
						MakeShared<FPreviewProfileController>()),
				InProvider);
		}
	};
}

SNvidiaWaveWorksEditorViewport::SNvidiaWaveWorksEditorViewport()
{
	PreviewScene =
		MakeShared<FAdvancedPreviewScene>(FPreviewScene::ConstructionValues());
}

SNvidiaWaveWorksEditorViewport::~SNvidiaWaveWorksEditorViewport()
{
	if (PreviewActor)
	{
		if (!IsEngineExitRequested() && IsValid(PreviewActor))
		{
			PreviewActor->Destroy();
		}
		PreviewActor = nullptr;
	}
}

void SNvidiaWaveWorksEditorViewport::Construct(const FArguments& InArgs)
{
	ToolkitWeak = InArgs._WaveWorksEditorToolkit;
	const TSharedPtr<FNvidiaWaveWorksAssetEditorToolkit> Toolkit =
		ToolkitWeak.Pin();
	check(Toolkit.IsValid());

	SEditorViewport::Construct(SEditorViewport::FArguments());

	PreviewScene->SetFloorVisibility(false);
	PreviewActor = PreviewScene->GetWorld()->SpawnActor<ANvidiaWaveWorksPreviewActor>();
	check(PreviewActor);
	PreviewActor->SetWaveWorksAsset(Toolkit->GetEditedAsset());

	const float PreviewSizeCentimeters =
		Toolkit->GetEditedAsset()->PreviewSizeMeters * 100.0f;
	EditorViewportClient->MoveViewportCamera(
		FVector(-0.75f * PreviewSizeCentimeters, 0.0f, 0.45f * PreviewSizeCentimeters),
		FRotator(-28.0f, 0.0f, 0.0f));
}

TSharedRef<SEditorViewport>
SNvidiaWaveWorksEditorViewport::GetViewportWidget()
{
	return SharedThis(this);
}

TSharedPtr<FExtender> SNvidiaWaveWorksEditorViewport::GetExtenders() const
{
	return MakeShared<FExtender>();
}

void SNvidiaWaveWorksEditorViewport::OnFloatingButtonClicked()
{
}

void SNvidiaWaveWorksEditorViewport::AddReferencedObjects(
	FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(PreviewActor);
}

FString SNvidiaWaveWorksEditorViewport::GetReferencerName() const
{
	return TEXT("SNvidiaWaveWorksEditorViewport");
}

void SNvidiaWaveWorksEditorViewport::SetSimulationPaused(bool bPaused)
{
	if (PreviewActor)
	{
		PreviewActor->SetSimulationPaused(bPaused);
	}
}

void SNvidiaWaveWorksEditorViewport::ResetSimulation()
{
	if (PreviewActor)
	{
		PreviewActor->ResetSimulation();
	}
}

TSharedRef<FEditorViewportClient>
SNvidiaWaveWorksEditorViewport::MakeEditorViewportClient()
{
	EditorViewportClient =
		MakeShared<FNvidiaWaveWorksEditorViewportClient>(
			PreviewScene.Get(),
			SharedThis(this));
	return EditorViewportClient.ToSharedRef();
}

TSharedPtr<SWidget> SNvidiaWaveWorksEditorViewport::MakeViewportToolbar()
{
	return SNew(
		NvidiaWaveWorksEditorViewportLocals::SViewportToolbar,
		SharedThis(this));
}

void SNvidiaWaveWorksEditorViewport::PopulateViewportOverlays(
	TSharedRef<SOverlay> Overlay)
{
	Overlay->AddSlot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Bottom)
		.Padding(10.0f)
		[
			SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush(TEXT("ToolPanel.GroupBorder")))
				.Padding(FMargin(8.0f, 5.0f))
				[
					SNew(STextBlock)
						.Text_Lambda(
							[this]()
							{
								const FString Status = PreviewActor
									? PreviewActor->GetSimulationStatus()
									: TEXT("Preview actor is unavailable");
								return FText::FromString(
									FString::Printf(
										TEXT("NVIDIA WaveWorks | %s"),
										*Status));
							})
				]
		];
}

FNvidiaWaveWorksEditorViewportClient::FNvidiaWaveWorksEditorViewportClient(
	FPreviewScene* InPreviewScene,
	const TWeakPtr<SEditorViewport>& InViewportWidget)
	: FEditorViewportClient(nullptr, InPreviewScene, InViewportWidget)
{
	bSetListenerPosition = false;
	SetRealtime(true);
	EngineShowFlags.Grid = false;
	SetViewMode(VMI_Lit);
}

void FNvidiaWaveWorksEditorViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);
	PreviewScene->GetWorld()->Tick(
		IsRealtime() ? LEVELTICK_All : LEVELTICK_TimeOnly,
		DeltaSeconds);
}
