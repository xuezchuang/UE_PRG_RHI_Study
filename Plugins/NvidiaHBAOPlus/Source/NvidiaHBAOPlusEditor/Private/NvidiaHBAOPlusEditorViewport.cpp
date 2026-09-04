#include "NvidiaHBAOPlusEditorViewport.h"

#include "AdvancedPreviewScene.h"
#include "Components/StaticMeshComponent.h"
#include "CoreGlobals.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "INvidiaHBAOPlusModule.h"
#include "NvidiaHBAOPlusAssetEditorToolkit.h"
#include "PreviewProfileController.h"
#include "Styling/AppStyle.h"
#include "UObject/UObjectGlobals.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"

namespace NvidiaHBAOPlusEditorViewportLocals
{
	class SViewportToolbar final
		: public SCommonEditorViewportToolbarBase
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

SNvidiaHBAOPlusEditorViewport::SNvidiaHBAOPlusEditorViewport()
{
	PreviewScene =
		MakeShared<FAdvancedPreviewScene>(
			FPreviewScene::ConstructionValues());
}

SNvidiaHBAOPlusEditorViewport::~SNvidiaHBAOPlusEditorViewport()
{
	for (AActor* Actor : PreviewActors)
	{
		if (!IsEngineExitRequested() && IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	PreviewActors.Reset();
}

void SNvidiaHBAOPlusEditorViewport::Construct(
	const FArguments& InArgs)
{
	ToolkitWeak = InArgs._HBAOPlusEditorToolkit;
	check(ToolkitWeak.IsValid());

	SEditorViewport::Construct(SEditorViewport::FArguments());

	PreviewScene->SetFloorVisibility(false);
	BuildPreviewScene();
	EditorViewportClient->MoveViewportCamera(
		FVector(-480.0, -480.0, 320.0),
		FRotator(-22.0f, 45.0f, 0.0f));
}

TSharedRef<SEditorViewport>
SNvidiaHBAOPlusEditorViewport::GetViewportWidget()
{
	return SharedThis(this);
}

TSharedPtr<FExtender>
SNvidiaHBAOPlusEditorViewport::GetExtenders() const
{
	return MakeShared<FExtender>();
}

void SNvidiaHBAOPlusEditorViewport::OnFloatingButtonClicked()
{
}

void SNvidiaHBAOPlusEditorViewport::AddReferencedObjects(
	FReferenceCollector& Collector)
{
	for (TObjectPtr<AActor>& Actor : PreviewActors)
	{
		Collector.AddReferencedObject(Actor);
	}
}

FString SNvidiaHBAOPlusEditorViewport::GetReferencerName() const
{
	return TEXT("SNvidiaHBAOPlusEditorViewport");
}

TSharedRef<FEditorViewportClient>
SNvidiaHBAOPlusEditorViewport::MakeEditorViewportClient()
{
	EditorViewportClient =
		MakeShared<FNvidiaHBAOPlusEditorViewportClient>(
			PreviewScene.Get(),
			SharedThis(this));
	return EditorViewportClient.ToSharedRef();
}

TSharedPtr<SWidget>
SNvidiaHBAOPlusEditorViewport::MakeViewportToolbar()
{
	return SNew(
		NvidiaHBAOPlusEditorViewportLocals::SViewportToolbar,
		SharedThis(this));
}

void SNvidiaHBAOPlusEditorViewport::PopulateViewportOverlays(
	TSharedRef<SOverlay> Overlay)
{
	Overlay->AddSlot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Bottom)
		.Padding(10.0f)
		[
			SNew(SBorder)
				.BorderImage(
					FAppStyle::GetBrush(
						TEXT("ToolPanel.GroupBorder")))
				.Padding(FMargin(8.0f, 5.0f))
				[
					SNew(STextBlock)
						.Text_Lambda(
							[]()
							{
								const FString Status =
									INvidiaHBAOPlusModule::IsAvailable()
									? INvidiaHBAOPlusModule::Get().
										GetStatus()
									: TEXT(
										"Runtime module is unavailable");
								return FText::FromString(
									FString::Printf(
										TEXT("NVIDIA HBAO+ | %s"),
										*Status));
							})
				]
		];
}

void SNvidiaHBAOPlusEditorViewport::BuildPreviewScene()
{
	SpawnPreviewMesh(
		TEXT("/Engine/BasicShapes/Plane.Plane"),
		FTransform(
			FRotator::ZeroRotator,
			FVector::ZeroVector,
			FVector(8.0, 8.0, 1.0)));
	SpawnPreviewMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"),
		FTransform(
			FRotator(0.0f, 15.0f, 0.0f),
			FVector(0.0, 0.0, 50.0),
			FVector(1.15, 1.15, 1.0)));
	SpawnPreviewMesh(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"),
		FTransform(
			FRotator::ZeroRotator,
			FVector(160.0, 30.0, 55.0),
			FVector(1.1)));
	SpawnPreviewMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"),
		FTransform(
			FRotator(0.0f, -20.0f, 0.0f),
			FVector(35.0, 170.0, 95.0),
			FVector(0.65, 0.65, 1.9)));
}

AActor* SNvidiaHBAOPlusEditorViewport::SpawnPreviewMesh(
	const TCHAR* MeshPath,
	const FTransform& Transform)
{
	UStaticMesh* StaticMesh =
		LoadObject<UStaticMesh>(nullptr, MeshPath);
	if (StaticMesh == nullptr || PreviewScene->GetWorld() == nullptr)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.ObjectFlags = RF_Transient;
	AStaticMeshActor* Actor =
		PreviewScene->GetWorld()->SpawnActor<AStaticMeshActor>(
			AStaticMeshActor::StaticClass(),
			Transform,
			SpawnParameters);
	if (Actor == nullptr)
	{
		return nullptr;
	}

	UStaticMeshComponent* MeshComponent =
		Actor->GetStaticMeshComponent();
	MeshComponent->SetMobility(EComponentMobility::Movable);
	MeshComponent->SetStaticMesh(StaticMesh);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewActors.Add(Actor);
	return Actor;
}

FNvidiaHBAOPlusEditorViewportClient::
	FNvidiaHBAOPlusEditorViewportClient(
		FPreviewScene* InPreviewScene,
		const TWeakPtr<SEditorViewport>& InViewportWidget)
	: FEditorViewportClient(
		nullptr,
		InPreviewScene,
		InViewportWidget)
{
	bSetListenerPosition = false;
	SetRealtime(true);
	EngineShowFlags.Grid = false;
	EngineShowFlags.SetAmbientOcclusion(false);
	SetViewMode(VMI_Lit);
}

void FNvidiaHBAOPlusEditorViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);
	PreviewScene->GetWorld()->Tick(
		IsRealtime() ? LEVELTICK_All : LEVELTICK_TimeOnly,
		DeltaSeconds);
}
