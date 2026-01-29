# Quick Start Guide - D3D12 RHI Access

This guide shows the simplest way to start using the D3D12RHIAccess plugin in your Unreal Engine plugin.

## 1. Enable the Plugin

Add to your `.uproject` file:

```json
{
    "Name": "D3D12RHIAccess",
    "Enabled": true
}
```

Or enable it through the Plugin Browser in the Unreal Editor.

## 2. Add Dependency to Your Plugin

In your plugin's `YourPlugin.Build.cs`:

```csharp
PrivateDependencyModuleNames.AddRange(
    new string[]
    {
        "Core",
        "RHI",
        "D3D12RHIAccess",  // Add this line
        // ... other dependencies
    }
);
```

## 3. Use in Your Code

### Simple Example

```cpp
#include "D3D12RHIAccessHelper.h"

void YourClass::YourFunction()
{
    // Create the helper
    FD3D12RHIAccessHelper D3D12Helper;
    
    // Check if D3D12 is available
    if (D3D12Helper.IsAvailable())
    {
        // Get the D3D12 device
        ID3D12Device* Device = D3D12Helper.GetDevice();
        
        // Get the command queue
        ID3D12CommandQueue* CommandQueue = D3D12Helper.GetCommandQueue();
        
        // Now you can use these with any D3D12-based SDK
        if (Device && CommandQueue)
        {
            // Use NVIDIA SDK, AMD SDK, Intel SDK, etc.
            // Example: InitializeYourSDK(Device, CommandQueue);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("D3D12 is not available"));
    }
}
```

### Full Example with Error Handling

```cpp
#include "D3D12RHIAccessHelper.h"

bool YourClass::InitializeD3D12Feature()
{
    // Step 1: Check if module is loaded
    if (!FModuleManager::Get().IsModuleLoaded("D3D12RHIAccess"))
    {
        UE_LOG(LogTemp, Error, TEXT("D3D12RHIAccess module not loaded"));
        return false;
    }
    
    // Step 2: Create helper
    FD3D12RHIAccessHelper D3D12Helper;
    
    // Step 3: Check availability
    if (!D3D12Helper.IsAvailable())
    {
        UE_LOG(LogTemp, Error, TEXT("D3D12 not available - check if D3D12 RHI is enabled"));
        return false;
    }
    
    // Step 4: Get interfaces
    ID3D12Device* Device = D3D12Helper.GetDevice();
    ID3D12CommandQueue* Queue = D3D12Helper.GetCommandQueue();
    
    if (!Device || !Queue)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get D3D12 interfaces"));
        return false;
    }
    
    // Step 5: Use the interfaces
    UE_LOG(LogTemp, Log, TEXT("Successfully got D3D12 Device and Queue!"));
    
    // Initialize your SDK here
    // YourSDK::Initialize(Device, Queue);
    
    return true;
}
```

## 4. Platform-Specific Code

If you need platform-specific code:

```cpp
#include "D3D12RHIAccessHelper.h"

void YourClass::PlatformSpecificFunction()
{
#if WITH_D3D12_RHI
    // This code only compiles on Windows
    FD3D12RHIAccessHelper D3D12Helper;
    
    if (D3D12Helper.IsAvailable())
    {
        ID3D12Device* Device = D3D12Helper.GetDevice();
        
        // D3D12-specific code here
        D3D12_FEATURE_DATA_D3D12_OPTIONS Options = {};
        Device->CheckFeatureSupport(
            D3D12_FEATURE_D3D12_OPTIONS,
            &Options,
            sizeof(Options)
        );
    }
#else
    // Fallback for other platforms
    UE_LOG(LogTemp, Warning, TEXT("D3D12 not available on this platform"));
#endif
}
```

## 5. Use in Render Commands

For rendering operations, enqueue commands on the render thread:

```cpp
#include "D3D12RHIAccessHelper.h"

void YourClass::RenderSomething()
{
    ENQUEUE_RENDER_COMMAND(MyD3D12Command)(
        [](FRHICommandListImmediate& RHICmdList)
        {
            FD3D12RHIAccessHelper D3D12Helper;
            
            if (D3D12Helper.IsAvailable())
            {
                ID3D12Device* Device = D3D12Helper.GetDevice();
                ID3D12GraphicsCommandList* CmdList = D3D12Helper.GetGraphicsCommandList();
                
                // Execute D3D12 commands on render thread
                if (Device && CmdList)
                {
                    // Your rendering code here
                }
            }
        }
    );
}
```

## Common Use Cases

### 1. Initialize Third-Party SDK

```cpp
bool InitSDK()
{
    FD3D12RHIAccessHelper D3D12;
    if (D3D12.IsAvailable())
    {
        return ThirdPartySDK::Init(D3D12.GetDevice());
    }
    return false;
}
```

### 2. Query Device Capabilities

```cpp
void CheckCapabilities()
{
    FD3D12RHIAccessHelper D3D12;
    if (D3D12.IsAvailable())
    {
        ID3D12Device* Device = D3D12.GetDevice();
        D3D12_FEATURE_DATA_D3D12_OPTIONS Options;
        Device->CheckFeatureSupport(
            D3D12_FEATURE_D3D12_OPTIONS,
            &Options,
            sizeof(Options)
        );
        
        UE_LOG(LogTemp, Log, TEXT("Resource Binding Tier: %d"),
            (int)Options.ResourceBindingTier);
    }
}
```

### 3. Create Custom D3D12 Resources

```cpp
void CreateCustomResource()
{
    FD3D12RHIAccessHelper D3D12;
    if (D3D12.IsAvailable())
    {
        ID3D12Device* Device = D3D12.GetDevice();
        
        D3D12_RESOURCE_DESC Desc = {};
        // Fill in resource description
        
        ID3D12Resource* Resource = nullptr;
        HRESULT hr = Device->CreateCommittedResource(
            &HeapProps,
            D3D12_HEAP_FLAG_NONE,
            &Desc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&Resource)
        );
        
        if (SUCCEEDED(hr))
        {
            // Use the resource
        }
    }
}
```

## Troubleshooting

### Issue: "D3D12RHIAccess module not loaded"
**Solution**: Ensure the plugin is enabled in your `.uproject` file

### Issue: "D3D12 not available"
**Solution**: Enable D3D12 in Project Settings > Windows > Default RHI

### Issue: Compile errors about ID3D12Device
**Solution**: Add `WITH_D3D12_RHI` guards around D3D12-specific code

### Issue: Crashes when accessing device
**Solution**: Always check if device is not nullptr before using

## Best Practices

1. **Always check availability** before using D3D12 interfaces
2. **Don't store** D3D12 pointers long-term - get them when needed
3. **Use on render thread** when performing rendering operations
4. **Handle errors gracefully** - not all platforms support D3D12
5. **Test on multiple GPUs** - behavior may vary

## Next Steps

- Read the full [README.md](README.md) for detailed API documentation
- Check [NVIDIA_WAVEWORKS_GUIDE.md](NVIDIA_WAVEWORKS_GUIDE.md) for a complete integration example
- Look at the example implementation in `Plugins/TPViewport/Private/D3D12IntegrationExample.cpp`

## Support

For issues or questions:
1. Check the README.md documentation
2. Look at the example implementations
3. Consult Unreal Engine D3D12 RHI documentation
