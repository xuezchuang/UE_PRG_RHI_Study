#include "NvidiaWaveWorksSimulation.h"

#include "Async/Async.h"
#include "DynamicRHI.h"
#include "ID3D12DynamicRHI.h"
#include "NvidiaWaveWorksAsset.h"
#include "RHICommandList.h"
#include "RenderingThread.h"

#if WITH_NVIDIA_WAVEWORKS
THIRD_PARTY_INCLUDES_START
#include "waveworks/GFSDK_WaveWorks.h"
THIRD_PARTY_INCLUDES_END
#endif

DEFINE_LOG_CATEGORY_STATIC(LogNvidiaWaveWorksSimulation, Log, All);

namespace NvidiaWaveWorksSimulationLocals
{
#if WITH_NVIDIA_WAVEWORKS
	FCriticalSection LibraryMutex;
	int32 LibraryReferenceCount = 0;

	bool AcquireLibrary(FString& OutError)
	{
		FScopeLock Lock(&LibraryMutex);

		if (LibraryReferenceCount == 0)
		{
			const gfsdk_wwresult Result = GFSDK_WaveWorks_Init(nullptr, GFSDK_WAVEWORKS_API_GUID);
			if (Result != gfsdk_wwresult_OK)
			{
				OutError = FString::Printf(TEXT("GFSDK_WaveWorks_Init failed (%d)"), static_cast<int32>(Result));
				return false;
			}
		}

		++LibraryReferenceCount;
		return true;
	}

	void ReleaseLibrary()
	{
		FScopeLock Lock(&LibraryMutex);
		check(LibraryReferenceCount > 0);

		--LibraryReferenceCount;
		if (LibraryReferenceCount == 0)
		{
			GFSDK_WaveWorks_Release();
		}
	}

	GFSDK_WaveWorks_Simulation_DetailLevel ToSdkDetailLevel(int32 DetailLevel)
	{
		switch (DetailLevel)
		{
		case 0:
			return GFSDK_WaveWorks_Simulation_DetailLevel_Normal;
		case 2:
			return GFSDK_WaveWorks_Simulation_DetailLevel_Extreme;
		default:
			return GFSDK_WaveWorks_Simulation_DetailLevel_High;
		}
	}

	gfsdk_float2 ToSdkDirection(FVector2f Direction)
	{
		if (!Direction.Normalize())
		{
			Direction = FVector2f(1.0f, 0.0f);
		}
		return {Direction.X, Direction.Y};
	}

	void FillSdkConfiguration(
		const FNvidiaWaveWorksSimulationSettings& Source,
		GFSDK_WaveWorks_Wind_Waves_Simulation_Settings& OutSettings,
		GFSDK_WaveWorks_Wind_Waves_Simulation_Parameters& OutParameters)
	{
		OutSettings = {};
		OutSettings.detail_level = ToSdkDetailLevel(Source.DetailLevel);
		OutSettings.simulation_api = GFSDK_WaveWorks_Simulation_API_Compute;
		OutSettings.simulation_period = Source.SimulationPeriod;
		OutSettings.use_Beaufort_scale = Source.bUseBeaufortScale;
		OutSettings.enable_CPU_driven_displacement_calculation = true;
		OutSettings.enable_GPU_driven_displacement_calculation = false;
		OutSettings.num_readback_FIFO_entries = 1;
		OutSettings.CPU_simulation_threading_model =
			GFSDK_WaveWorks_Simulation_CPU_Threading_Model_Automatic;
		OutSettings.num_GPUs = 1;
		OutSettings.enable_GPU_timers = true;
		OutSettings.enable_CPU_timers = true;

		OutParameters = {};
		OutParameters.base_wind_direction = ToSdkDirection(Source.BaseWindDirection);
		OutParameters.base_wind_speed = Source.BaseWindSpeed;
		OutParameters.base_wind_distance = Source.BaseWindDistance;
		OutParameters.base_wind_dependency = Source.BaseWindDependency;
		OutParameters.base_spectrum_peaking = Source.BaseSpectrumPeaking;
		OutParameters.base_small_waves_cutoff_length = 0.0f;
		OutParameters.base_small_waves_cutoff_power = 0.0f;
		OutParameters.base_amplitude_multiplier = Source.BaseAmplitudeMultiplier;

		OutParameters.swell_wind_direction = ToSdkDirection(Source.SwellWindDirection);
		OutParameters.swell_wind_speed = Source.SwellWindSpeed;
		OutParameters.swell_wind_distance = Source.SwellWindDistance;
		OutParameters.swell_wind_dependency = Source.SwellWindDependency;
		OutParameters.swell_spectrum_peaking = Source.SwellSpectrumPeaking;
		OutParameters.swell_small_waves_cutoff_length = 60.0f;
		OutParameters.swell_small_waves_cutoff_power = 1.0f;
		OutParameters.swell_amplitude_multiplier = Source.SwellAmplitudeMultiplier;

		OutParameters.lateral_multiplier = Source.LateralMultiplier;
		OutParameters.time_scale = Source.TimeScale;
		OutParameters.uv_warping_amplitude = Source.UVWarpingAmplitude;
		OutParameters.uv_warping_frequency = Source.UVWarpingFrequency;
		OutParameters.foam_whitecaps_threshold = Source.FoamWhitecapsThreshold;
		OutParameters.foam_generation_threshold = Source.FoamGenerationThreshold;
		OutParameters.foam_generation_amount = Source.FoamGenerationAmount;
		OutParameters.foam_dissipation_speed = Source.FoamDissipationSpeed;
		OutParameters.foam_falloff_speed = Source.FoamFalloffSpeed;
	}
#endif
}

struct FNvidiaWaveWorksSimulation::FImpl
{
#if WITH_NVIDIA_WAVEWORKS
	GFSDK_WaveWorks_Wind_Waves_SimulationHandle Simulation = nullptr;
	bool bLibraryAcquired = false;
	bool bLoggedRunning = false;
	uint32 AppliedSettingsRevision = 0;
	uint64 LastKickId = 0;
#endif
};

FNvidiaWaveWorksSimulationSettings FNvidiaWaveWorksSimulationSettings::FromAsset(
	const UNvidiaWaveWorksAsset& Asset)
{
	FNvidiaWaveWorksSimulationSettings Result;
	Result.DetailLevel = static_cast<int32>(Asset.DetailLevel);
	Result.SimulationPeriod = Asset.SimulationPeriod;
	Result.bUseBeaufortScale = Asset.bUseBeaufortScale;
	Result.TimeScale = Asset.TimeScale;

	Result.BaseWindDirection = FVector2f(Asset.BaseWindDirection);
	Result.BaseWindSpeed = Asset.BaseWindSpeed;
	Result.BaseWindDistance = Asset.BaseWindDistance;
	Result.BaseWindDependency = Asset.BaseWindDependency;
	Result.BaseSpectrumPeaking = Asset.BaseSpectrumPeaking;
	Result.BaseAmplitudeMultiplier = Asset.BaseAmplitudeMultiplier;

	Result.SwellWindDirection = FVector2f(Asset.SwellWindDirection);
	Result.SwellWindSpeed = Asset.SwellWindSpeed;
	Result.SwellWindDistance = Asset.SwellWindDistance;
	Result.SwellWindDependency = Asset.SwellWindDependency;
	Result.SwellSpectrumPeaking = Asset.SwellSpectrumPeaking;
	Result.SwellAmplitudeMultiplier = Asset.SwellAmplitudeMultiplier;

	Result.LateralMultiplier = Asset.LateralMultiplier;
	Result.UVWarpingAmplitude = Asset.UVWarpingAmplitude;
	Result.UVWarpingFrequency = Asset.UVWarpingFrequency;

	Result.FoamWhitecapsThreshold = Asset.FoamWhitecapsThreshold;
	Result.FoamGenerationThreshold = Asset.FoamGenerationThreshold;
	Result.FoamGenerationAmount = Asset.FoamGenerationAmount;
	Result.FoamDissipationSpeed = Asset.FoamDissipationSpeed;
	Result.FoamFalloffSpeed = Asset.FoamFalloffSpeed;
	return Result;
}

FNvidiaWaveWorksSimulation::FNvidiaWaveWorksSimulation()
	: Impl(MakeUnique<FImpl>())
{
}

FNvidiaWaveWorksSimulation::~FNvidiaWaveWorksSimulation()
{
#if WITH_NVIDIA_WAVEWORKS
	ensureMsgf(
		Impl == nullptr || (Impl->Simulation == nullptr && !Impl->bLibraryAcquired),
		TEXT("WaveWorks simulation was destroyed without Shutdown()"));
#endif
}

void FNvidiaWaveWorksSimulation::UpdateSettings(
	const FNvidiaWaveWorksSimulationSettings& InSettings)
{
	FScopeLock Lock(&SettingsMutex);
	Settings = InSettings;
	++SettingsRevision;
}

void FNvidiaWaveWorksSimulation::RequestReset()
{
	bResetRequested.Store(true);
}

bool FNvidiaWaveWorksSimulation::RequestDisplacements(
	double AbsoluteTimeSeconds,
	TArray<FVector2f>&& SamplePositionsMeters,
	FDisplacementCallback&& Completion)
{
	if (bShutdown.Load() || SamplePositionsMeters.IsEmpty())
	{
		return false;
	}

	bool bExpected = false;
	if (!bRequestInFlight.CompareExchange(bExpected, true))
	{
		return false;
	}

	const TSharedRef<FNvidiaWaveWorksSimulation, ESPMode::ThreadSafe> Self = AsShared();
	ENQUEUE_RENDER_COMMAND(NvidiaWaveWorksSampleDisplacements)(
		[Self,
		 AbsoluteTimeSeconds,
		 SamplePositionsMeters = MoveTemp(SamplePositionsMeters),
		 Completion = MoveTemp(Completion)](FRHICommandListImmediate&) mutable
		{
			TArray<FVector3f> Displacements;
			const bool bSucceeded =
				!Self->bShutdown.Load() &&
				Self->Tick_RenderThread(AbsoluteTimeSeconds, SamplePositionsMeters, Displacements);

			AsyncTask(
				ENamedThreads::GameThread,
				[Self,
				 bSucceeded,
				 Displacements = MoveTemp(Displacements),
				 Completion = MoveTemp(Completion)]() mutable
				{
					Self->bRequestInFlight.Store(false);
					if (bSucceeded && !Self->bShutdown.Load())
					{
						Completion(MoveTemp(Displacements));
					}
				});
		});

	return true;
}

void FNvidiaWaveWorksSimulation::Shutdown()
{
	if (bShutdown.Exchange(true))
	{
		return;
	}

	if (IsInRenderingThread())
	{
		Destroy_RenderThread();
		return;
	}

	const TSharedRef<FNvidiaWaveWorksSimulation, ESPMode::ThreadSafe> Self = AsShared();
	ENQUEUE_RENDER_COMMAND(NvidiaWaveWorksShutdown)(
		[Self](FRHICommandListImmediate&)
		{
			Self->Destroy_RenderThread();
		});
	FlushRenderingCommands();
}

FString FNvidiaWaveWorksSimulation::GetStatus() const
{
	FScopeLock Lock(&StatusMutex);
	return Status;
}

bool FNvidiaWaveWorksSimulation::Tick_RenderThread(
	double AbsoluteTimeSeconds,
	const TArray<FVector2f>& SamplePositionsMeters,
	TArray<FVector3f>& OutDisplacements)
{
	check(IsInRenderingThread());

#if !WITH_NVIDIA_WAVEWORKS
	SetStatus(TEXT("WaveWorks SDK is not available for this platform"));
	return false;
#else
	if (!GDynamicRHI || FCString::Stricmp(GDynamicRHI->GetName(), TEXT("D3D12")) != 0)
	{
		SetStatus(TEXT("WaveWorks requires the D3D12 RHI"));
		return false;
	}

	ID3D12DynamicRHI* D3D12RHI = GetID3D12DynamicRHI();
	if (!D3D12RHI)
	{
		SetStatus(TEXT("ID3D12DynamicRHI is unavailable"));
		return false;
	}

	ID3D12Device* Device = D3D12RHI->RHIGetDevice(0);
	ID3D12CommandQueue* Queue = D3D12RHI->RHIGetCommandQueue();
	if (!Device || !Queue)
	{
		SetStatus(TEXT("UE did not expose a D3D12 device and graphics queue"));
		return false;
	}

	FNvidiaWaveWorksSimulationSettings SettingsSnapshot;
	uint32 SettingsRevisionSnapshot = 0;
	{
		FScopeLock Lock(&SettingsMutex);
		SettingsSnapshot = Settings;
		SettingsRevisionSnapshot = SettingsRevision;
	}

	if (bResetRequested.Exchange(false) && Impl->Simulation)
	{
		GFSDK_WaveWorks_Wind_Waves_Simulation_Destroy(Impl->Simulation);
		Impl->Simulation = nullptr;
		Impl->AppliedSettingsRevision = 0;
	}

	if (!Impl->bLibraryAcquired)
	{
		FString Error;
		if (!NvidiaWaveWorksSimulationLocals::AcquireLibrary(Error))
		{
			SetStatus(Error);
			return false;
		}
		Impl->bLibraryAcquired = true;
		UE_LOG(
			LogNvidiaWaveWorksSimulation,
			Log,
			TEXT("Initialized %s"),
			ANSI_TO_TCHAR(GFSDK_WaveWorks_GetBuildString()));
	}

	GFSDK_WaveWorks_Wind_Waves_Simulation_Settings SdkSettings = {};
	GFSDK_WaveWorks_Wind_Waves_Simulation_Parameters SdkParameters = {};
	NvidiaWaveWorksSimulationLocals::FillSdkConfiguration(
		SettingsSnapshot,
		SdkSettings,
		SdkParameters);

	if (!Impl->Simulation)
	{
		const gfsdk_wwresult CreateResult =
			GFSDK_WaveWorks_Wind_Waves_Simulation_CreateDirectX12(
				Device,
				Queue,
				true,
				SdkSettings,
				SdkParameters,
				&Impl->Simulation);
		if (CreateResult != gfsdk_wwresult_OK || !Impl->Simulation)
		{
			SetStatus(FString::Printf(
				TEXT("CreateDirectX12 failed (%d)"),
				static_cast<int32>(CreateResult)));
			return false;
		}
		Impl->AppliedSettingsRevision = SettingsRevisionSnapshot;
	}
	else if (Impl->AppliedSettingsRevision != SettingsRevisionSnapshot)
	{
		const gfsdk_wwresult UpdateResult =
			GFSDK_WaveWorks_Wind_Waves_Simulation_UpdateProperties(
				Impl->Simulation,
				SdkSettings,
				SdkParameters);
		if (UpdateResult != gfsdk_wwresult_OK)
		{
			SetStatus(FString::Printf(
				TEXT("UpdateProperties failed (%d)"),
				static_cast<int32>(UpdateResult)));
			return false;
		}
		Impl->AppliedSettingsRevision = SettingsRevisionSnapshot;
	}

	GFSDK_WaveWorks_Wind_Waves_Simulation_SetTime(Impl->Simulation, AbsoluteTimeSeconds);
	const gfsdk_wwresult KickResult =
		GFSDK_WaveWorks_Wind_Waves_Simulation_Kick(Impl->Simulation, &Impl->LastKickId);
	if (KickResult != gfsdk_wwresult_OK)
	{
		SetStatus(FString::Printf(TEXT("Simulation_Kick failed (%d)"), static_cast<int32>(KickResult)));
		return false;
	}

	// Advance each asynchronous pipeline by at most one item per editor frame.
	GFSDK_WaveWorks_Wind_Waves_Simulation_AdvanceStagingCursor(Impl->Simulation, false);
	GFSDK_WaveWorks_Wind_Waves_Simulation_AdvanceReadbackCursor(Impl->Simulation, false);

	uint64 ReadbackKickId = 0;
	if (GFSDK_WaveWorks_Wind_Waves_Simulation_GetReadbackCursor(
			Impl->Simulation,
			&ReadbackKickId) != gfsdk_wwresult_OK)
	{
		SetStatus(TEXT("Priming WaveWorks GPU/readback pipeline"));
		return false;
	}

	TArray<gfsdk_float2> SdkPositions;
	SdkPositions.SetNumUninitialized(SamplePositionsMeters.Num());
	for (int32 Index = 0; Index < SamplePositionsMeters.Num(); ++Index)
	{
		SdkPositions[Index] =
			{SamplePositionsMeters[Index].X, SamplePositionsMeters[Index].Y};
	}

	TArray<gfsdk_float4> SdkDisplacements;
	SdkDisplacements.SetNumUninitialized(SamplePositionsMeters.Num());
	const gfsdk_wwresult DisplacementResult =
		GFSDK_WaveWorks_Wind_Waves_Simulation_GetDisplacements(
			Impl->Simulation,
			SdkPositions.GetData(),
			SdkDisplacements.GetData(),
			static_cast<uint32>(SdkPositions.Num()),
			false);
	if (DisplacementResult != gfsdk_wwresult_OK)
	{
		SetStatus(FString::Printf(
			TEXT("GetDisplacements failed (%d)"),
			static_cast<int32>(DisplacementResult)));
		return false;
	}

	OutDisplacements.SetNumUninitialized(SdkDisplacements.Num());
	for (int32 Index = 0; Index < SdkDisplacements.Num(); ++Index)
	{
		OutDisplacements[Index] = FVector3f(
			SdkDisplacements[Index].x,
			SdkDisplacements[Index].y,
			SdkDisplacements[Index].z);
	}

	GFSDK_WaveWorks_Wind_Waves_Simulation_Stats Stats = {};
	GFSDK_WaveWorks_Wind_Waves_Simulation_GetStats(Impl->Simulation, Stats);
	if (!Impl->bLoggedRunning)
	{
		UE_LOG(
			LogNvidiaWaveWorksSimulation,
			Log,
			TEXT("WaveWorks D3D12 simulation and CPU displacement readback are running (%d samples)"),
			OutDisplacements.Num());
		Impl->bLoggedRunning = true;
	}
	SetStatus(FString::Printf(
		TEXT("Running | kick %llu | readback %llu | GPU %.2f ms"),
		static_cast<unsigned long long>(Impl->LastKickId),
		static_cast<unsigned long long>(ReadbackKickId),
		Stats.GPU_total_time));
	return true;
#endif
}

void FNvidiaWaveWorksSimulation::Destroy_RenderThread()
{
	check(IsInRenderingThread());

#if WITH_NVIDIA_WAVEWORKS
	if (Impl && Impl->Simulation)
	{
		GFSDK_WaveWorks_Wind_Waves_Simulation_Destroy(Impl->Simulation);
		Impl->Simulation = nullptr;
	}

	if (Impl && Impl->bLibraryAcquired)
	{
		NvidiaWaveWorksSimulationLocals::ReleaseLibrary();
		Impl->bLibraryAcquired = false;
	}
#endif
	SetStatus(TEXT("Stopped"));
}

void FNvidiaWaveWorksSimulation::SetStatus(const FString& InStatus)
{
	FScopeLock Lock(&StatusMutex);
	Status = InStatus;
}
