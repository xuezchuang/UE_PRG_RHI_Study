#include "Containers/Ticker.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformProcess.h"
#include "HAL/IConsoleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "NvidiaWaveWorksAsset.h"
#include "NvidiaWaveWorksSimulation.h"

DEFINE_LOG_CATEGORY_STATIC(LogNvidiaWaveWorks, Log, All);

class FNvidiaWaveWorksModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
#if PLATFORM_WINDOWS && WITH_NVIDIA_WAVEWORKS
		const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("NvidiaWaveWorks"));
		if (!Plugin.IsValid())
		{
			UE_LOG(LogNvidiaWaveWorks, Error, TEXT("Could not resolve the NvidiaWaveWorks plugin directory"));
			return;
		}

		const FString DllName = TEXT("NVWaveWorks_shared.dll");
		const TArray<FString> DllCandidates = {
			FPaths::Combine(
				Plugin->GetBaseDir(),
				TEXT("Binaries/ThirdParty/WaveWorks/Win64"),
				DllName),
			FPaths::Combine(
				Plugin->GetBaseDir(),
				TEXT("ThirdParty/WaveWorks/Bin/Win64"),
				DllName),
			FPaths::Combine(FPlatformProcess::BaseDir(), DllName)};

		FString LoadedDllPath;
		for (const FString& DllPath : DllCandidates)
		{
			if (IFileManager::Get().FileExists(*DllPath))
			{
				WaveWorksDllHandle = FPlatformProcess::GetDllHandle(*DllPath);
				if (WaveWorksDllHandle != nullptr)
				{
					LoadedDllPath = DllPath;
					break;
				}
			}
		}

		if (WaveWorksDllHandle == nullptr)
		{
			UE_LOG(
				LogNvidiaWaveWorks,
				Error,
				TEXT("Failed to load WaveWorks SDK DLL from the plugin or executable directories"));
			return;
		}
		UE_LOG(LogNvidiaWaveWorks, Log, TEXT("Loaded WaveWorks SDK DLL: %s"), *LoadedDllPath);
#endif

#if !UE_BUILD_SHIPPING
		SmokeTestCommand = IConsoleManager::Get().RegisterConsoleCommand(
			TEXT("NvidiaWaveWorks.SmokeTest"),
			TEXT("Runs a short D3D12 WaveWorks simulation and displacement-readback test."),
			FConsoleCommandDelegate::CreateRaw(
				this,
				&FNvidiaWaveWorksModule::StartSmokeTest),
			ECVF_Default);
#endif

		UE_LOG(LogNvidiaWaveWorks, Log, TEXT("NVIDIA WaveWorks runtime module loaded"));
	}

	virtual void ShutdownModule() override
	{
#if !UE_BUILD_SHIPPING
		StopSmokeTest();
		if (SmokeTestCommand != nullptr)
		{
			IConsoleManager::Get().UnregisterConsoleObject(
				SmokeTestCommand,
				false);
			SmokeTestCommand = nullptr;
		}
#endif

		UE_LOG(LogNvidiaWaveWorks, Log, TEXT("NVIDIA WaveWorks runtime module unloaded"));

#if PLATFORM_WINDOWS && WITH_NVIDIA_WAVEWORKS
		if (WaveWorksDllHandle != nullptr)
		{
			FPlatformProcess::FreeDllHandle(WaveWorksDllHandle);
			WaveWorksDllHandle = nullptr;
		}
#endif
	}

private:
#if !UE_BUILD_SHIPPING
	void StartSmokeTest()
	{
		if (SmokeTestSimulation.IsValid())
		{
			UE_LOG(LogNvidiaWaveWorks, Warning, TEXT("WaveWorks smoke test is already running"));
			return;
		}

		SmokeTestSimulation =
			MakeShared<FNvidiaWaveWorksSimulation, ESPMode::ThreadSafe>();
		SmokeTestSimulation->UpdateSettings(
			FNvidiaWaveWorksSimulationSettings::FromAsset(
				*GetDefault<UNvidiaWaveWorksAsset>()));
		bSmokeTestReadbackSucceeded.Store(false);
		SmokeTestStartTime = FPlatformTime::Seconds();
		SmokeTestTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
			FTickerDelegate::CreateRaw(
				this,
				&FNvidiaWaveWorksModule::TickSmokeTest));

		UE_LOG(LogNvidiaWaveWorks, Log, TEXT("WaveWorks D3D12 smoke test started"));
	}

	bool TickSmokeTest(float)
	{
		if (!SmokeTestSimulation.IsValid())
		{
			SmokeTestTickerHandle.Reset();
			return false;
		}

		if (bSmokeTestReadbackSucceeded.Load())
		{
			UE_LOG(
				LogNvidiaWaveWorks,
				Log,
				TEXT("WaveWorks D3D12 smoke test passed: %s"),
				*SmokeTestSimulation->GetStatus());
			SmokeTestSimulation->Shutdown();
			SmokeTestSimulation.Reset();
			SmokeTestTickerHandle.Reset();
			return false;
		}

		const double ElapsedSeconds =
			FPlatformTime::Seconds() - SmokeTestStartTime;
		if (ElapsedSeconds > 20.0)
		{
			UE_LOG(
				LogNvidiaWaveWorks,
				Error,
				TEXT("WaveWorks D3D12 smoke test timed out: %s"),
				*SmokeTestSimulation->GetStatus());
			SmokeTestSimulation->Shutdown();
			SmokeTestSimulation.Reset();
			SmokeTestTickerHandle.Reset();
			return false;
		}

		TArray<FVector2f> Positions;
		Positions.Reserve(64);
		for (int32 Y = 0; Y < 8; ++Y)
		{
			for (int32 X = 0; X < 8; ++X)
			{
				Positions.Emplace(
					(static_cast<float>(X) - 3.5f) * 4.0f,
					(static_cast<float>(Y) - 3.5f) * 4.0f);
			}
		}

		SmokeTestSimulation->RequestDisplacements(
			ElapsedSeconds,
			MoveTemp(Positions),
			[this](TArray<FVector3f>&& Displacements)
			{
				if (Displacements.Num() == 64)
				{
					bSmokeTestReadbackSucceeded.Store(true);
				}
			});
		return true;
	}

	void StopSmokeTest()
	{
		if (SmokeTestTickerHandle.IsValid())
		{
			FTSTicker::GetCoreTicker().RemoveTicker(SmokeTestTickerHandle);
			SmokeTestTickerHandle.Reset();
		}

		if (SmokeTestSimulation.IsValid())
		{
			SmokeTestSimulation->Shutdown();
			SmokeTestSimulation.Reset();
		}
	}

	IConsoleObject* SmokeTestCommand = nullptr;
	TSharedPtr<FNvidiaWaveWorksSimulation, ESPMode::ThreadSafe> SmokeTestSimulation;
	FTSTicker::FDelegateHandle SmokeTestTickerHandle;
	TAtomic<bool> bSmokeTestReadbackSucceeded = false;
	double SmokeTestStartTime = 0.0;
#endif

	void* WaveWorksDllHandle = nullptr;
};

IMPLEMENT_MODULE(FNvidiaWaveWorksModule, NvidiaWaveWorks)
