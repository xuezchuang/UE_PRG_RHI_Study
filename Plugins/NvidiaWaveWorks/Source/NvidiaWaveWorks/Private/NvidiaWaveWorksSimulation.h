#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"

class UNvidiaWaveWorksAsset;

struct FNvidiaWaveWorksSimulationSettings
{
	int32 DetailLevel = 1;
	float SimulationPeriod = 1000.0f;
	bool bUseBeaufortScale = true;
	float TimeScale = 1.0f;

	FVector2f BaseWindDirection = FVector2f(1.0f, 0.0f);
	float BaseWindSpeed = 4.7f;
	float BaseWindDistance = 0.1f;
	float BaseWindDependency = 1.0f;
	float BaseSpectrumPeaking = 3.3f;
	float BaseAmplitudeMultiplier = 1.0f;

	FVector2f SwellWindDirection = FVector2f(0.0f, 1.0f);
	float SwellWindSpeed = 1.5f;
	float SwellWindDistance = 520.0f;
	float SwellWindDependency = 1.0f;
	float SwellSpectrumPeaking = 10.0f;
	float SwellAmplitudeMultiplier = 1.0f;

	float LateralMultiplier = 1.0f;
	float UVWarpingAmplitude = 0.03f;
	float UVWarpingFrequency = 2.0f;

	float FoamWhitecapsThreshold = 0.5f;
	float FoamGenerationThreshold = 0.37f;
	float FoamGenerationAmount = 0.12f;
	float FoamDissipationSpeed = 0.6f;
	float FoamFalloffSpeed = 0.985f;

	static FNvidiaWaveWorksSimulationSettings FromAsset(const UNvidiaWaveWorksAsset& Asset);
};

/**
 * Render-thread owner for WaveWorks. The editor/game thread submits immutable
 * parameter and sample snapshots and receives displacements asynchronously.
 */
class FNvidiaWaveWorksSimulation final
	: public TSharedFromThis<FNvidiaWaveWorksSimulation, ESPMode::ThreadSafe>
{
public:
	using FDisplacementCallback = TFunction<void(TArray<FVector3f>&&)>;

	FNvidiaWaveWorksSimulation();
	~FNvidiaWaveWorksSimulation();

	void UpdateSettings(const FNvidiaWaveWorksSimulationSettings& InSettings);
	void RequestReset();

	bool RequestDisplacements(
		double AbsoluteTimeSeconds,
		TArray<FVector2f>&& SamplePositionsMeters,
		FDisplacementCallback&& Completion);

	void Shutdown();
	FString GetStatus() const;

private:
	struct FImpl;

	bool Tick_RenderThread(
		double AbsoluteTimeSeconds,
		const TArray<FVector2f>& SamplePositionsMeters,
		TArray<FVector3f>& OutDisplacements);
	void Destroy_RenderThread();
	void SetStatus(const FString& InStatus);

	TUniquePtr<FImpl> Impl;

	mutable FCriticalSection SettingsMutex;
	FNvidiaWaveWorksSimulationSettings Settings;
	uint32 SettingsRevision = 1;

	mutable FCriticalSection StatusMutex;
	FString Status = TEXT("Waiting for the first simulation tick");

	TAtomic<bool> bRequestInFlight = false;
	TAtomic<bool> bShutdown = false;
	TAtomic<bool> bResetRequested = false;
};
