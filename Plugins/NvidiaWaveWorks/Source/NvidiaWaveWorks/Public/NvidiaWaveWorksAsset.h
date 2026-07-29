#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "NvidiaWaveWorksAsset.generated.h"

UENUM(BlueprintType)
enum class ENvidiaWaveWorksDetailLevel : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	High UMETA(DisplayName = "High"),
	Extreme UMETA(DisplayName = "Extreme")
};

/**
 * Serializable WaveWorks parameters. Live SDK handles deliberately do not live
 * on the asset; they are owned by the render-thread simulation object.
 */
UCLASS(BlueprintType)
class NVIDIAWAVEWORKS_API UNvidiaWaveWorksAsset : public UObject
{
	GENERATED_BODY()

public:
	UNvidiaWaveWorksAsset();

	UPROPERTY(EditAnywhere, Category = "Simulation")
	ENvidiaWaveWorksDetailLevel DetailLevel = ENvidiaWaveWorksDetailLevel::High;

	UPROPERTY(EditAnywhere, Category = "Simulation", meta = (ClampMin = "10.0", UIMin = "100.0", UIMax = "4000.0", Units = "m"))
	float SimulationPeriod = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Simulation")
	bool bUseBeaufortScale = true;

	UPROPERTY(EditAnywhere, Category = "Simulation", meta = (ClampMin = "0.0", ClampMax = "4.0", UIMin = "0.0", UIMax = "2.0"))
	float TimeScale = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Base Wind")
	FVector2D BaseWindDirection = FVector2D(1.0, 0.0);

	UPROPERTY(EditAnywhere, Category = "Base Wind", meta = (ClampMin = "0.0", UIMin = "0.0", UIMax = "12.0"))
	float BaseWindSpeed = 4.7f;

	UPROPERTY(EditAnywhere, Category = "Base Wind", meta = (ClampMin = "0.001", UIMin = "0.01", UIMax = "1000.0", Units = "km"))
	float BaseWindDistance = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Base Wind", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BaseWindDependency = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Base Wind", meta = (ClampMin = "1.0", UIMin = "3.3", UIMax = "10.0"))
	float BaseSpectrumPeaking = 3.3f;

	UPROPERTY(EditAnywhere, Category = "Base Wind", meta = (ClampMin = "0.0", UIMin = "0.0", UIMax = "4.0"))
	float BaseAmplitudeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Swell")
	FVector2D SwellWindDirection = FVector2D(0.0, 1.0);

	UPROPERTY(EditAnywhere, Category = "Swell", meta = (ClampMin = "0.0", UIMin = "0.0", UIMax = "12.0"))
	float SwellWindSpeed = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Swell", meta = (ClampMin = "0.001", UIMin = "1.0", UIMax = "1000.0", Units = "km"))
	float SwellWindDistance = 520.0f;

	UPROPERTY(EditAnywhere, Category = "Swell", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SwellWindDependency = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Swell", meta = (ClampMin = "1.0", UIMin = "3.3", UIMax = "12.0"))
	float SwellSpectrumPeaking = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Swell", meta = (ClampMin = "0.0", UIMin = "0.0", UIMax = "4.0"))
	float SwellAmplitudeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Surface", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LateralMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Surface", meta = (ClampMin = "0.0", ClampMax = "0.05"))
	float UVWarpingAmplitude = 0.03f;

	UPROPERTY(EditAnywhere, Category = "Surface", meta = (ClampMin = "1.0", ClampMax = "3.0"))
	float UVWarpingFrequency = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Foam", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FoamWhitecapsThreshold = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Foam", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FoamGenerationThreshold = 0.37f;

	UPROPERTY(EditAnywhere, Category = "Foam", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FoamGenerationAmount = 0.12f;

	UPROPERTY(EditAnywhere, Category = "Foam", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FoamDissipationSpeed = 0.6f;

	UPROPERTY(EditAnywhere, Category = "Foam", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FoamFalloffSpeed = 0.985f;

	UPROPERTY(EditAnywhere, Category = "Preview", meta = (ClampMin = "8", ClampMax = "128", UIMin = "16", UIMax = "96"))
	int32 PreviewGridResolution = 48;

	UPROPERTY(EditAnywhere, Category = "Preview", meta = (ClampMin = "1.0", UIMin = "10.0", UIMax = "200.0", Units = "m"))
	float PreviewSizeMeters = 80.0f;

	UPROPERTY(EditAnywhere, Category = "Preview", meta = (ClampMin = "1.0", ClampMax = "60.0"))
	float PreviewUpdatesPerSecond = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Preview")
	FLinearColor PreviewColor = FLinearColor(0.02f, 0.32f, 0.78f, 1.0f);

#if WITH_EDITOR
	DECLARE_MULTICAST_DELEGATE(FOnSettingsChanged);

	FOnSettingsChanged& OnSettingsChanged()
	{
		return SettingsChangedDelegate;
	}

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	FOnSettingsChanged SettingsChangedDelegate;
#endif
};
