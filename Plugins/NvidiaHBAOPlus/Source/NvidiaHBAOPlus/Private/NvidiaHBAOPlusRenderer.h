#pragma once

#include "CoreMinimal.h"

class FPostOpaqueRenderParameters;
class UNvidiaHBAOPlusAsset;

struct FNvidiaHBAOPlusRenderSettings
{
	bool bEnabled = true;
	bool bVisualizeAO = false;
	float Radius = 2.0f;
	float Bias = 0.1f;
	float SmallScaleAO = 1.0f;
	float LargeScaleAO = 1.0f;
	float PowerExponent = 2.0f;
	int32 StepCount = 4;
	int32 DepthStorage = 16;
	bool bEnableBlur = true;
	int32 BlurRadius = 4;
	float BlurSharpness = 16.0f;

	static FNvidiaHBAOPlusRenderSettings FromAsset(
		const UNvidiaHBAOPlusAsset& Asset);
};

class FNvidiaHBAOPlusRenderer final
{
public:
	FNvidiaHBAOPlusRenderer();
	~FNvidiaHBAOPlusRenderer();

	void UpdateSettings(const FNvidiaHBAOPlusRenderSettings& InSettings);
	void AddPostOpaquePass(FPostOpaqueRenderParameters& Parameters);
	void RequestContextReset();
	void Shutdown();

	FString GetStatus() const;

private:
	struct FImpl;

	void ReleaseContexts_RenderThread();
	void SetStatus(const FString& InStatus);

	TUniquePtr<FImpl> Impl;

	mutable FCriticalSection SettingsMutex;
	FNvidiaHBAOPlusRenderSettings Settings;

	mutable FCriticalSection StatusMutex;
	FString Status = TEXT("Waiting for the first rendered view");

	TAtomic<bool> bShutdown = false;
};
