#include "D3D12IntegrationExample.h"
#include "D3D12RHIAccessHelper.h"

#if PLATFORM_WINDOWS && WITH_D3D12_RHI

#include "Windows/AllowWindowsPlatformTypes.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#include "Windows/HideWindowsPlatformTypes.h"

#endif


DEFINE_LOG_CATEGORY_STATIC(LogD3D12Integration, Log, All);

FD3D12IntegrationExample::FD3D12IntegrationExample()
	: bIsInitialized(false)
{
}

FD3D12IntegrationExample::~FD3D12IntegrationExample()
{
	Shutdown();
}

bool FD3D12IntegrationExample::Initialize()
{
	if (bIsInitialized)
	{
		return true;
	}

	UE_LOG(LogD3D12Integration, Log, TEXT("Initializing D3D12 Integration Example"));

	// Check if D3D12 RHI Access module is loaded
	if (!FModuleManager::Get().IsModuleLoaded("D3D12RHIAccess"))
	{
		UE_LOG(LogD3D12Integration, Warning, TEXT("D3D12RHIAccess module is not loaded"));
		return false;
	}

	// Use the helper class to access D3D12 interfaces
	FD3D12RHIAccessHelper D3D12Helper;
	
	if (!D3D12Helper.IsAvailable())
	{
		UE_LOG(LogD3D12Integration, Warning, TEXT("D3D12 RHI is not available on this platform or not in use"));
		return false;
	}

#if WITH_D3D12_RHI
	// Get the D3D12 device
	ID3D12Device* Device = D3D12Helper.GetDevice();
	if (!Device)
	{
		UE_LOG(LogD3D12Integration, Error, TEXT("Failed to get D3D12 Device"));
		return false;
	}

	// Get the command queue
	ID3D12CommandQueue* CommandQueue = D3D12Helper.GetCommandQueue();
	if (!CommandQueue)
	{
		UE_LOG(LogD3D12Integration, Error, TEXT("Failed to get D3D12 Command Queue"));
		return false;
	}

	UE_LOG(LogD3D12Integration, Log, TEXT("Successfully obtained D3D12 Device and Command Queue"));
	
	// Here you would initialize NVIDIA WaveWorks or other D3D12-based SDK
	// Example (use the constant name from your NVIDIA SDK version):
	// GFSDK_WaveWorks_InitD3D12(Device, NULL, GFSDK_WAVEWORKS_API_GUID);
	
	bIsInitialized = true;
	return true;
#else
	UE_LOG(LogD3D12Integration, Warning, TEXT("D3D12 RHI is not enabled in this build"));
	return false;
#endif
}

void FD3D12IntegrationExample::Shutdown()
{
	if (!bIsInitialized)
	{
		return;
	}

	UE_LOG(LogD3D12Integration, Log, TEXT("Shutting down D3D12 Integration Example"));

	// Cleanup NVIDIA SDK or other D3D12 resources here
	// Example:
	// GFSDK_WaveWorks_ReleaseD3D12();

	bIsInitialized = false;
}

void FD3D12IntegrationExample::ExampleD3D12Usage()
{
	if (!bIsInitialized)
	{
		UE_LOG(LogD3D12Integration, Warning, TEXT("D3D12 Integration not initialized"));
		return;
	}

#if WITH_D3D12_RHI
	// Example of accessing D3D12 interfaces during rendering
	FD3D12RHIAccessHelper D3D12Helper;
	
	if (D3D12Helper.IsAvailable())
	{
		ID3D12Device* Device = D3D12Helper.GetDevice();
		ID3D12CommandQueue* CommandQueue = D3D12Helper.GetCommandQueue();
		
		if (Device && CommandQueue)
		{
			UE_LOG(LogD3D12Integration, Log, TEXT("D3D12 Device and Command Queue are available for use"));
			
			// Example: Query device properties
			D3D12_FEATURE_DATA_D3D12_OPTIONS Options = {};
			HRESULT hr = Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &Options, sizeof(Options));
			if (SUCCEEDED(hr))
			{
				UE_LOG(LogD3D12Integration, Log, TEXT("D3D12 Device supports Tier %d Resource Binding"), 
					(int32)Options.ResourceBindingTier);
			}
			
			// Here you would call NVIDIA SDK functions:
			// - Update WaveWorks simulation
			// - Render water surfaces
			// - Apply GPU effects
			// etc.
		}
	}
#endif
}
