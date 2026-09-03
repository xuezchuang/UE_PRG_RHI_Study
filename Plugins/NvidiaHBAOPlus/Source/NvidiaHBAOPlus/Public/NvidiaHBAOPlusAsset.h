#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "NvidiaHBAOPlusAsset.generated.h"

UENUM(BlueprintType)
enum class ENvidiaHBAOPlusStepCount : uint8
{
	Steps4 UMETA(DisplayName = "4 Steps"),
	Steps8 UMETA(DisplayName = "8 Steps")
};

UENUM(BlueprintType)
enum class ENvidiaHBAOPlusBlurRadius : uint8
{
	Radius2 UMETA(DisplayName = "2 Pixels"),
	Radius4 UMETA(DisplayName = "4 Pixels")
};

UENUM(BlueprintType)
enum class ENvidiaHBAOPlusDepthStorage : uint8
{
	FP16 UMETA(DisplayName = "FP16 (Faster)"),
	FP32 UMETA(DisplayName = "FP32")
};

/**
 * Serializable HBAO+ render settings. The live NVIDIA context and D3D12
 * descriptors are owned by the render-thread renderer, never by this asset.
 */
UCLASS(BlueprintType)
class NVIDIAHBAOPLUS_API UNvidiaHBAOPlusAsset : public UObject
{
	GENERATED_BODY()

public:
	UNvidiaHBAOPlusAsset();

	UPROPERTY(EditAnywhere, Category = "Rendering")
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, Category = "Rendering")
	bool bVisualizeAO = false;

	UPROPERTY(
		EditAnywhere,
		Category = "Ambient Occlusion",
		meta = (ClampMin = "0.01", UIMin = "0.1", UIMax = "8.0", Units = "m"))
	float Radius = 2.0f;

	UPROPERTY(
		EditAnywhere,
		Category = "Ambient Occlusion",
		meta = (ClampMin = "0.0", ClampMax = "0.5", UIMin = "0.0", UIMax = "0.5"))
	float Bias = 0.1f;

	UPROPERTY(
		EditAnywhere,
		Category = "Ambient Occlusion",
		meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float SmallScaleAO = 1.0f;

	UPROPERTY(
		EditAnywhere,
		Category = "Ambient Occlusion",
		meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float LargeScaleAO = 1.0f;

	UPROPERTY(
		EditAnywhere,
		Category = "Ambient Occlusion",
		meta = (ClampMin = "1.0", ClampMax = "4.0"))
	float PowerExponent = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Quality")
	ENvidiaHBAOPlusStepCount StepCount = ENvidiaHBAOPlusStepCount::Steps4;

	UPROPERTY(EditAnywhere, Category = "Quality")
	ENvidiaHBAOPlusDepthStorage DepthStorage =
		ENvidiaHBAOPlusDepthStorage::FP16;

	UPROPERTY(EditAnywhere, Category = "Blur")
	bool bEnableBlur = true;

	UPROPERTY(EditAnywhere, Category = "Blur", meta = (EditCondition = "bEnableBlur"))
	ENvidiaHBAOPlusBlurRadius BlurRadius =
		ENvidiaHBAOPlusBlurRadius::Radius4;

	UPROPERTY(
		EditAnywhere,
		Category = "Blur",
		meta = (
			EditCondition = "bEnableBlur",
			ClampMin = "0.0",
			UIMin = "0.0",
			UIMax = "64.0"))
	float BlurSharpness = 16.0f;

#if WITH_EDITOR
	DECLARE_MULTICAST_DELEGATE(FOnSettingsChanged);

	FOnSettingsChanged& OnSettingsChanged()
	{
		return SettingsChangedDelegate;
	}

	void NotifySettingsChanged();
	virtual void PostEditChangeProperty(
		FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	FOnSettingsChanged SettingsChangedDelegate;
#endif
};
