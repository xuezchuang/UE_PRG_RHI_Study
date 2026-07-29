#include "NvidiaWaveWorksPreviewActor.h"

#include "KismetProceduralMeshLibrary.h"
#include "Materials/MaterialInterface.h"
#include "NvidiaWaveWorksAsset.h"
#include "NvidiaWaveWorksSimulation.h"
#include "ProceduralMeshComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NvidiaWaveWorksPreviewActor)

ANvidiaWaveWorksPreviewActor::ANvidiaWaveWorksPreviewActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	PreviewMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("WaveWorksPreviewMesh"));
	SetRootComponent(PreviewMesh);
	PreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewMesh->SetCanEverAffectNavigation(false);
	PreviewMesh->bUseAsyncCooking = false;
}

void ANvidiaWaveWorksPreviewActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!WaveWorksAsset || bSimulationPaused)
	{
		return;
	}

	SimulationTime += DeltaSeconds;
	UpdateAccumulator += DeltaSeconds;

	const float UpdateInterval =
		1.0f / FMath::Max(1.0f, WaveWorksAsset->PreviewUpdatesPerSecond);
	if (UpdateAccumulator >= UpdateInterval)
	{
		UpdateAccumulator = FMath::Fmod(UpdateAccumulator, UpdateInterval);
		RequestSimulationUpdate();
	}
}

void ANvidiaWaveWorksPreviewActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
#if WITH_EDITOR
	if (WaveWorksAsset)
	{
		WaveWorksAsset->OnSettingsChanged().RemoveAll(this);
	}
#endif

	ShutdownSimulation();
	Super::EndPlay(EndPlayReason);
}

void ANvidiaWaveWorksPreviewActor::SetWaveWorksAsset(UNvidiaWaveWorksAsset* InAsset)
{
	if (WaveWorksAsset == InAsset)
	{
		return;
	}

#if WITH_EDITOR
	if (WaveWorksAsset)
	{
		WaveWorksAsset->OnSettingsChanged().RemoveAll(this);
	}
#endif

	WaveWorksAsset = InAsset;

#if WITH_EDITOR
	if (WaveWorksAsset)
	{
		WaveWorksAsset->OnSettingsChanged().AddUObject(
			this,
			&ANvidiaWaveWorksPreviewActor::HandleAssetSettingsChanged);
	}
#endif

	if (!Simulation.IsValid())
	{
		Simulation = MakeShared<FNvidiaWaveWorksSimulation, ESPMode::ThreadSafe>();
	}

	HandleAssetSettingsChanged();
}

void ANvidiaWaveWorksPreviewActor::SetSimulationPaused(bool bInPaused)
{
	bSimulationPaused = bInPaused;
}

void ANvidiaWaveWorksPreviewActor::ResetSimulation()
{
	SimulationTime = 0.0;
	if (Simulation.IsValid())
	{
		Simulation->RequestReset();
	}
}

FString ANvidiaWaveWorksPreviewActor::GetSimulationStatus() const
{
	return Simulation.IsValid()
		? Simulation->GetStatus()
		: TEXT("Simulation not created");
}

void ANvidiaWaveWorksPreviewActor::HandleAssetSettingsChanged()
{
	if (!WaveWorksAsset)
	{
		return;
	}

	if (!Simulation.IsValid())
	{
		Simulation = MakeShared<FNvidiaWaveWorksSimulation, ESPMode::ThreadSafe>();
	}

	Simulation->UpdateSettings(
		FNvidiaWaveWorksSimulationSettings::FromAsset(*WaveWorksAsset));
	RebuildPreviewMesh();
}

void ANvidiaWaveWorksPreviewActor::RebuildPreviewMesh()
{
	if (!WaveWorksAsset || !PreviewMesh)
	{
		return;
	}

	const int32 Resolution =
		FMath::Clamp(WaveWorksAsset->PreviewGridResolution, 8, 128);
	const float SizeMeters = FMath::Max(1.0f, WaveWorksAsset->PreviewSizeMeters);
	const float SizeCentimeters = SizeMeters * 100.0f;

	BaseVertices.Reset(Resolution * Resolution);
	DeformedVertices.Reset(Resolution * Resolution);
	UVs.Reset(Resolution * Resolution);
	Normals.Reset(Resolution * Resolution);
	Tangents.Reset(Resolution * Resolution);
	VertexColors.Reset(Resolution * Resolution);
	SamplePositionsMeters.Reset(Resolution * Resolution);
	Triangles.Reset((Resolution - 1) * (Resolution - 1) * 6);

	for (int32 Y = 0; Y < Resolution; ++Y)
	{
		const float V = static_cast<float>(Y) / static_cast<float>(Resolution - 1);
		for (int32 X = 0; X < Resolution; ++X)
		{
			const float U = static_cast<float>(X) / static_cast<float>(Resolution - 1);
			const float MeterX = (U - 0.5f) * SizeMeters;
			const float MeterY = (V - 0.5f) * SizeMeters;

			BaseVertices.Add(FVector(
				(U - 0.5f) * SizeCentimeters,
				(V - 0.5f) * SizeCentimeters,
				0.0f));
			UVs.Add(FVector2D(U, V));
			Normals.Add(FVector::UpVector);
			Tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
			VertexColors.Add(WaveWorksAsset->PreviewColor);
			SamplePositionsMeters.Add(FVector2f(MeterX, MeterY));
		}
	}

	for (int32 Y = 0; Y < Resolution - 1; ++Y)
	{
		for (int32 X = 0; X < Resolution - 1; ++X)
		{
			const int32 I00 = Y * Resolution + X;
			const int32 I10 = I00 + 1;
			const int32 I01 = I00 + Resolution;
			const int32 I11 = I01 + 1;

			Triangles.Append({I00, I10, I01, I10, I11, I01});
		}
	}

	DeformedVertices = BaseVertices;
	PreviewMesh->ClearAllMeshSections();
	PreviewMesh->CreateMeshSection_LinearColor(
		0,
		DeformedVertices,
		Triangles,
		Normals,
		UVs,
		VertexColors,
		Tangents,
		false);

	UMaterialInterface* VertexColorMaterial = LoadObject<UMaterialInterface>(
		nullptr,
		TEXT("/Engine/EngineDebugMaterials/VertexColorMaterial.VertexColorMaterial"));
	if (VertexColorMaterial)
	{
		PreviewMesh->SetMaterial(0, VertexColorMaterial);
	}
}

void ANvidiaWaveWorksPreviewActor::RequestSimulationUpdate()
{
	if (!Simulation.IsValid() || SamplePositionsMeters.IsEmpty())
	{
		return;
	}

	TArray<FVector2f> Positions = SamplePositionsMeters;
	TWeakObjectPtr<ANvidiaWaveWorksPreviewActor> WeakThis(this);
	Simulation->RequestDisplacements(
		SimulationTime,
		MoveTemp(Positions),
		[WeakThis](TArray<FVector3f>&& Displacements)
		{
			if (WeakThis.IsValid())
			{
				WeakThis->ApplyDisplacements(MoveTemp(Displacements));
			}
		});
}

void ANvidiaWaveWorksPreviewActor::ApplyDisplacements(
	TArray<FVector3f>&& Displacements)
{
	if (!PreviewMesh || Displacements.Num() != BaseVertices.Num())
	{
		return;
	}

	DeformedVertices.SetNumUninitialized(BaseVertices.Num());
	for (int32 Index = 0; Index < BaseVertices.Num(); ++Index)
	{
		const FVector DisplacementCentimeters =
			FVector(Displacements[Index]) * 100.0;
		DeformedVertices[Index] =
			BaseVertices[Index] + DisplacementCentimeters;
	}

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(
		DeformedVertices,
		Triangles,
		UVs,
		Normals,
		Tangents);

	PreviewMesh->UpdateMeshSection_LinearColor(
		0,
		DeformedVertices,
		Normals,
		UVs,
		VertexColors,
		Tangents);
}

void ANvidiaWaveWorksPreviewActor::ShutdownSimulation()
{
	if (Simulation.IsValid())
	{
		Simulation->Shutdown();
		Simulation.Reset();
	}
}
