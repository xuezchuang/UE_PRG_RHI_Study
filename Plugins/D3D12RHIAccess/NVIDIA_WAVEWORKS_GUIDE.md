# NVIDIA WaveWorks Integration Guide

This guide explains how to integrate NVIDIA WaveWorks with Unreal Engine 5.2 using the D3D12RHIAccess plugin.

## Prerequisites

1. **NVIDIA WaveWorks SDK**: Download from NVIDIA Developer website
2. **Visual Studio 2022**: Required for UE5.2 development
3. **Windows 10/11**: D3D12 support required
4. **Unreal Engine 5.2**: With D3D12 RHI enabled

## Step 1: Install NVIDIA WaveWorks SDK

1. Download the NVIDIA WaveWorks SDK from the NVIDIA Developer website
2. Extract the SDK to a location on your system (e.g., `C:/SDKs/WaveWorks`)
3. Note the paths to:
   - Include directory: `C:/SDKs/WaveWorks/include`
   - Library directory: `C:/SDKs/WaveWorks/lib/x64`
   - DLL files: `C:/SDKs/WaveWorks/bin/x64`

## Step 2: Create Your WaveWorks Plugin

### 2.1 Create Plugin Structure

Create a new plugin in your project's `Plugins` directory:

```
Plugins/
  WaveWorks/
    WaveWorks.uplugin
    Source/
      WaveWorks/
        WaveWorks.Build.cs
        Public/
          WaveWorksComponent.h
        Private/
          WaveWorksModule.cpp
          WaveWorksComponent.cpp
```

### 2.2 Create Plugin Descriptor (WaveWorks.uplugin)

```json
{
	"FileVersion": 3,
	"Version": 1,
	"VersionName": "1.0",
	"FriendlyName": "NVIDIA WaveWorks",
	"Description": "NVIDIA WaveWorks water simulation integration",
	"Category": "Rendering",
	"CreatedBy": "Your Name",
	"CanContainContent": true,
	"IsBetaVersion": false,
	"IsExperimentalVersion": false,
	"Installed": false,
	"Modules": [
		{
			"Name": "WaveWorks",
			"Type": "Runtime",
			"LoadingPhase": "Default",
			"PlatformAllowList": [ "Win64" ]
		}
	],
	"Plugins": [
		{
			"Name": "D3D12RHIAccess",
			"Enabled": true
		}
	]
}
```

### 2.3 Create Build Configuration (WaveWorks.Build.cs)

```csharp
using UnrealBuildTool;
using System.IO;

public class WaveWorks : ModuleRules
{
	public WaveWorks(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"RenderCore",
				"RHI",
				"D3D12RHIAccess"  // Required for D3D12 access
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects"
			}
		);

		// Configure NVIDIA WaveWorks SDK paths
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			string WaveWorksSDKPath = "C:/SDKs/WaveWorks"; // Update this path
			string IncludePath = Path.Combine(WaveWorksSDKPath, "include");
			string LibPath = Path.Combine(WaveWorksSDKPath, "lib/x64");

			PublicSystemIncludePaths.Add(IncludePath);
			PublicAdditionalLibraries.Add(Path.Combine(LibPath, "GFSDK_WaveWorks.x64.lib"));
			
			// Add DLL to runtime dependencies
			string DLLPath = Path.Combine(WaveWorksSDKPath, "bin/x64/GFSDK_WaveWorks.x64.dll");
			RuntimeDependencies.Add(DLLPath);
			
			PublicDefinitions.Add("WITH_WAVEWORKS=1");
		}
		else
		{
			PublicDefinitions.Add("WITH_WAVEWORKS=0");
		}
	}
}
```

## Step 3: Implement WaveWorks Component

### 3.1 Component Header (WaveWorksComponent.h)

```cpp
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WaveWorksComponent.generated.h"

#if WITH_WAVEWORKS
#include "GFSDK_WaveWorks.h"
#endif

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WAVEWORKS_API UWaveWorksComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UWaveWorksComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;

	// WaveWorks parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaveWorks")
	float WaveAmplitude = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaveWorks")
	float WindSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaveWorks")
	FVector2D WindDirection = FVector2D(1.0f, 0.0f);

private:
	bool InitializeWaveWorks();
	void ShutdownWaveWorks();
	void UpdateSimulation(float DeltaTime);

#if WITH_WAVEWORKS
	GFSDK_WaveWorks_SimulationHandle WaveWorksHandle;
	GFSDK_WaveWorks_SavestateHandle SavestateHandle;
	bool bIsInitialized;
#endif
};
```

### 3.2 Component Implementation (WaveWorksComponent.cpp)

```cpp
#include "WaveWorksComponent.h"
#include "D3D12RHIAccessHelper.h"

DEFINE_LOG_CATEGORY_STATIC(LogWaveWorks, Log, All);

UWaveWorksComponent::UWaveWorksComponent()
#if WITH_WAVEWORKS
	: WaveWorksHandle(nullptr)
	, SavestateHandle(nullptr)
	, bIsInitialized(false)
#endif
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWaveWorksComponent::BeginPlay()
{
	Super::BeginPlay();
	
#if WITH_WAVEWORKS
	InitializeWaveWorks();
#endif
}

void UWaveWorksComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
#if WITH_WAVEWORKS
	ShutdownWaveWorks();
#endif
	
	Super::EndPlay(EndPlayReason);
}

void UWaveWorksComponent::TickComponent(float DeltaTime, ELevelTick TickType, 
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if WITH_WAVEWORKS
	if (bIsInitialized)
	{
		UpdateSimulation(DeltaTime);
	}
#endif
}

#if WITH_WAVEWORKS
bool UWaveWorksComponent::InitializeWaveWorks()
{
	UE_LOG(LogWaveWorks, Log, TEXT("Initializing NVIDIA WaveWorks"));

	// Get D3D12 device using the D3D12RHIAccess plugin
	FD3D12RHIAccessHelper D3D12Helper;
	
	if (!D3D12Helper.IsAvailable())
	{
		UE_LOG(LogWaveWorks, Error, TEXT("D3D12 RHI is not available"));
		return false;
	}

	ID3D12Device* Device = D3D12Helper.GetDevice();
	if (!Device)
	{
		UE_LOG(LogWaveWorks, Error, TEXT("Failed to get D3D12 Device"));
		return false;
	}

	// Initialize WaveWorks library
	HRESULT hr = GFSDK_WaveWorks_InitD3D12(Device, NULL, GFSDK_WaveWorks_API_GUID);
	if (FAILED(hr))
	{
		UE_LOG(LogWaveWorks, Error, TEXT("Failed to initialize WaveWorks: 0x%08X"), hr);
		return false;
	}

	// Create simulation settings
	GFSDK_WaveWorks_Simulation_Settings SimSettings;
	GFSDK_WaveWorks_Simulation_Params SimParams;
	
	// Configure simulation
	SimSettings.fft_period = 1000.0f;
	SimSettings.readback_displacements = true;
	SimSettings.num_readback_FIFO_entries = 3;
	SimSettings.aniso_level = 4;
	SimSettings.CPU_simulation_threading_model = 
		GFSDK_WaveWorks_Simulation_CPU_Threading_Model_Automatic;
	SimSettings.use_Beaufort_scale = true;
	SimSettings.num_GPUs = 1;

	// Set up wave parameters
	SimParams.time_scale = 1.0f;
	SimParams.wave_amplitude = WaveAmplitude;
	SimParams.wind_dir.x = WindDirection.X;
	SimParams.wind_dir.y = WindDirection.Y;
	SimParams.wind_speed = WindSpeed;
	SimParams.wind_dependency = 0.98f;
	SimParams.choppy_scale = 1.0f;
	SimParams.small_wave_fraction = 0.0f;
	SimParams.foam_generation_threshold = 0.3f;
	SimParams.foam_generation_amount = 0.5f;
	SimParams.foam_dissipation_speed = 0.6f;
	SimParams.foam_falloff_speed = 0.985f;

	// Create simulation
	hr = GFSDK_WaveWorks_Simulation_CreateD3D12(
		SimSettings,
		SimParams,
		Device,
		&WaveWorksHandle
	);

	if (FAILED(hr))
	{
		UE_LOG(LogWaveWorks, Error, TEXT("Failed to create WaveWorks simulation: 0x%08X"), hr);
		GFSDK_WaveWorks_ReleaseD3D12();
		return false;
	}

	UE_LOG(LogWaveWorks, Log, TEXT("WaveWorks initialized successfully"));
	bIsInitialized = true;
	return true;
}

void UWaveWorksComponent::ShutdownWaveWorks()
{
	if (!bIsInitialized)
	{
		return;
	}

	UE_LOG(LogWaveWorks, Log, TEXT("Shutting down WaveWorks"));

	if (SavestateHandle)
	{
		GFSDK_WaveWorks_Savestate_Destroy(SavestateHandle);
		SavestateHandle = nullptr;
	}

	if (WaveWorksHandle)
	{
		GFSDK_WaveWorks_Simulation_Destroy(WaveWorksHandle);
		WaveWorksHandle = nullptr;
	}

	GFSDK_WaveWorks_ReleaseD3D12();
	bIsInitialized = false;
}

void UWaveWorksComponent::UpdateSimulation(float DeltaTime)
{
	if (!WaveWorksHandle)
	{
		return;
	}

	// Update simulation parameters
	GFSDK_WaveWorks_Simulation_Params params;
	params.time_scale = 1.0f;
	params.wave_amplitude = WaveAmplitude;
	params.wind_dir.x = WindDirection.X;
	params.wind_dir.y = WindDirection.Y;
	params.wind_speed = WindSpeed;
	params.wind_dependency = 0.98f;
	params.choppy_scale = 1.0f;
	params.small_wave_fraction = 0.0f;
	params.foam_generation_threshold = 0.3f;
	params.foam_generation_amount = 0.5f;
	params.foam_dissipation_speed = 0.6f;
	params.foam_falloff_speed = 0.985f;

	GFSDK_WaveWorks_Simulation_UpdateProperties(
		WaveWorksHandle,
		params,
		nullptr
	);

	// Kick off simulation for this frame
	FD3D12RHIAccessHelper D3D12Helper;
	ID3D12GraphicsCommandList* CommandList = D3D12Helper.GetGraphicsCommandList();
	
	if (CommandList)
	{
		GFSDK_WaveWorks_Simulation_KickD3D12(
			WaveWorksHandle,
			static_cast<double>(DeltaTime),
			CommandList,
			nullptr
		);
	}
}
#endif
```

## Step 4: Enable the Plugin in Your Project

1. Open your project's `.uproject` file
2. Add the WaveWorks plugin to the plugins list:

```json
{
	"Name": "WaveWorks",
	"Enabled": true
}
```

3. Regenerate project files
4. Build the project

## Step 5: Use in Blueprints

1. Create a new Actor Blueprint
2. Add the WaveWorks Component
3. Configure wave parameters in the Details panel:
   - Wave Amplitude
   - Wind Speed
   - Wind Direction
4. Place the actor in your level

## Troubleshooting

### "D3D12 RHI is not available"

Ensure D3D12 is enabled in your project settings:
- Edit > Project Settings > Platforms > Windows > Default RHI: DirectX 12

### "Failed to initialize WaveWorks"

Check that:
1. NVIDIA WaveWorks DLL is in the correct location
2. The SDK version matches your API GUID
3. Your GPU supports D3D12

### Compilation Errors

Verify that:
1. D3D12RHIAccess plugin is enabled
2. WaveWorks SDK paths in Build.cs are correct
3. All required DLLs are in the binary directory

## Performance Tips

1. Use appropriate FFT resolution for your use case
2. Limit readback operations for better performance
3. Use LOD system for distant water surfaces
4. Consider multi-GPU support for high-end systems

## Additional Resources

- NVIDIA WaveWorks Documentation
- Unreal Engine RHI Documentation
- D3D12 Programming Guide

## License

This integration guide is for educational purposes. NVIDIA WaveWorks SDK has its own license terms.
