# D3D12 RHI Access Plugin

This plugin provides safe access to D3D12 RHI native interfaces for Unreal Engine plugins. It allows plugins to integrate with NVIDIA SDKs (like WaveWorks) and other D3D12-based libraries.

## Features

- Safe access to ID3D12Device
- Safe access to ID3D12CommandQueue
- Safe access to ID3D12GraphicsCommandList
- Platform-aware (Windows only)
- Easy-to-use helper class

## Installation

1. Copy the `D3D12RHIAccess` folder to your project's `Plugins` directory
2. Add the plugin to your project's `.uproject` file or enable it in the Plugin Browser
3. Regenerate your project files

## Usage

### In Your Plugin's Build.cs

Add `D3D12RHIAccess` to your plugin's dependencies:

```csharp
PublicDependencyModuleNames.AddRange(
    new string[]
    {
        "Core",
        "D3D12RHIAccess",
        // ... other dependencies
    }
);
```

### In Your Code

#### Using the Helper Class (Recommended)

```cpp
#include "D3D12RHIAccessHelper.h"

void MyRenderingFunction()
{
    FD3D12RHIAccessHelper D3D12Helper;
    
    if (D3D12Helper.IsAvailable())
    {
        ID3D12Device* Device = D3D12Helper.GetDevice();
        ID3D12CommandQueue* CommandQueue = D3D12Helper.GetCommandQueue();
        
        if (Device && CommandQueue)
        {
            // Use the D3D12 interfaces with NVIDIA SDK or other libraries
            // Example: Initialize NVIDIA WaveWorks
            // GFSDK_WaveWorks_InitD3D12(Device, ...);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("D3D12 RHI is not available"));
    }
}
```

#### Using the Module Interface Directly

```cpp
#include "ID3D12RHIAccessModule.h"
#include "Modules/ModuleManager.h"

void MyFunction()
{
    if (FModuleManager::Get().IsModuleLoaded("D3D12RHIAccess"))
    {
        ID3D12RHIAccessModule& Module = FModuleManager::GetModuleChecked<ID3D12RHIAccessModule>("D3D12RHIAccess");
        
        if (Module.IsD3D12Available())
        {
            ID3D12RHIAccessInterface* Access = Module.GetD3D12RHIAccessInterface();
            ID3D12Device* Device = Access->GetD3D12Device();
            
            // Use the device...
        }
    }
}
```

## NVIDIA WaveWorks Integration Example

Here's a complete example of how to integrate NVIDIA WaveWorks using this plugin:

```cpp
#include "D3D12RHIAccessHelper.h"

// Include NVIDIA WaveWorks SDK headers
#include "GFSDK_WaveWorks.h"

class FWaveWorksIntegration
{
public:
    bool Initialize()
    {
        FD3D12RHIAccessHelper D3D12Helper;
        
        if (!D3D12Helper.IsAvailable())
        {
            UE_LOG(LogTemp, Error, TEXT("D3D12 RHI is not available"));
            return false;
        }
        
        ID3D12Device* Device = D3D12Helper.GetDevice();
        if (!Device)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to get D3D12 Device"));
            return false;
        }
        
        // Initialize NVIDIA WaveWorks
        GFSDK_WaveWorks_InitD3D12(Device, NULL, GFSDK_WAVEWORKS_API_VERSION);
        
        // Create WaveWorks simulation
        GFSDK_WaveWorks_Simulation_Settings settings;
        // Configure settings...
        
        HRESULT hr = GFSDK_WaveWorks_Simulation_CreateD3D12(
            settings,
            Device,
            &WaveWorksHandle
        );
        
        if (FAILED(hr))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create WaveWorks simulation"));
            return false;
        }
        
        return true;
    }
    
    void Shutdown()
    {
        if (WaveWorksHandle)
        {
            GFSDK_WaveWorks_Simulation_Destroy(WaveWorksHandle);
            WaveWorksHandle = nullptr;
        }
    }
    
private:
    GFSDK_WaveWorks_SimulationHandle WaveWorksHandle = nullptr;
};
```

## Requirements

- Unreal Engine 5.2+
- Windows 64-bit platform
- D3D12 RHI enabled in project settings

## Platform Support

Currently, this plugin only supports Windows 64-bit with D3D12 RHI. On other platforms, the plugin will compile but the interfaces will return nullptr.

## Notes

- The D3D12 interfaces are owned by the RHI and should not be manually released
- Always check if the interfaces are available before using them
- The command list interface should be used with caution as it represents the current rendering state
- This plugin is designed for advanced users who need direct D3D12 access

## License

Copyright Epic Games, Inc. All Rights Reserved.
