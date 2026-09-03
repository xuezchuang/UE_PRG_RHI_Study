#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "NvidiaWaveWorksPreviewActor.generated.h"

class FNvidiaWaveWorksSimulation;
class UMaterialInterface;
class UNvidiaWaveWorksAsset;
class UProceduralMeshComponent;
struct FProcMeshTangent;

/** UE-rendered preview surface driven by NVIDIA WaveWorks displacement samples. */
UCLASS(Transient, NotBlueprintable)
class NVIDIAWAVEWORKS_API ANvidiaWaveWorksPreviewActor : public AActor
{
	GENERATED_BODY()

public:
	ANvidiaWaveWorksPreviewActor();

	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SetWaveWorksAsset(UNvidiaWaveWorksAsset* InAsset);
	void SetSimulationPaused(bool bInPaused);
	void ResetSimulation();
	FString GetSimulationStatus() const;

private:
	void HandleAssetSettingsChanged();
	void RebuildPreviewMesh();
	void RequestSimulationUpdate();
	void ApplyDisplacements(TArray<FVector3f>&& Displacements);
	void ShutdownSimulation();

	UPROPERTY(VisibleAnywhere, Transient, Category = "WaveWorks Preview")
	TObjectPtr<UProceduralMeshComponent> PreviewMesh;

	UPROPERTY(Transient)
	TObjectPtr<UNvidiaWaveWorksAsset> WaveWorksAsset;

	TSharedPtr<FNvidiaWaveWorksSimulation, ESPMode::ThreadSafe> Simulation;

	TArray<FVector> BaseVertices;
	TArray<FVector> DeformedVertices;
	TArray<int32> Triangles;
	TArray<FVector2D> UVs;
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;
	TArray<FLinearColor> VertexColors;
	TArray<FVector2f> SamplePositionsMeters;

	double SimulationTime = 0.0;
	float UpdateAccumulator = 0.0f;
	bool bSimulationPaused = false;
};
