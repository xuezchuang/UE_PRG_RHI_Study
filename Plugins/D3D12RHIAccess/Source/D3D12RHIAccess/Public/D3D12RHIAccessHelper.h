#pragma once

#include "CoreMinimal.h"
#include "ID3D12RHIAccessModule.h"
#include "Modules/ModuleManager.h"

/**
 * Helper class to easily access D3D12 RHI interfaces from plugins
 * 
 * Example usage:
 * 
 * #include "D3D12RHIAccessHelper.h"
 * 
 * void MyFunction()
 * {
 *     FD3D12RHIAccessHelper D3D12Helper;
 *     if (D3D12Helper.IsAvailable())
 *     {
 *         ID3D12Device* Device = D3D12Helper.GetDevice();
 *         ID3D12CommandQueue* CommandQueue = D3D12Helper.GetCommandQueue();
 *         
 *         // Use the D3D12 interfaces with NVIDIA SDK or other libraries
 *         // ...
 *     }
 * }
 */
class FD3D12RHIAccessHelper
{
public:
	FD3D12RHIAccessHelper()
		: Module(nullptr)
		, AccessInterface(nullptr)
	{
		if (FModuleManager::Get().IsModuleLoaded("D3D12RHIAccess"))
		{
			Module = &FModuleManager::GetModuleChecked<ID3D12RHIAccessModule>("D3D12RHIAccess");
			if (Module && Module->IsD3D12Available())
			{
				AccessInterface = Module->GetD3D12RHIAccessInterface();
			}
		}
	}

	/**
	 * Check if D3D12 RHI is available and accessible
	 */
	bool IsAvailable() const
	{
		return AccessInterface != nullptr;
	}

#if WITH_D3D12_RHI
	/**
	 * Get the D3D12 device
	 * @return ID3D12Device pointer or nullptr if not available
	 */
	ID3D12Device* GetDevice() const
	{
		return AccessInterface ? AccessInterface->GetD3D12Device() : nullptr;
	}

	/**
	 * Get the D3D12 command queue
	 * @return ID3D12CommandQueue pointer or nullptr if not available
	 */
	ID3D12CommandQueue* GetCommandQueue() const
	{
		return AccessInterface ? AccessInterface->GetD3D12CommandQueue() : nullptr;
	}

	/**
	 * Get the current D3D12 graphics command list
	 * @return ID3D12GraphicsCommandList pointer or nullptr if not available
	 */
	ID3D12GraphicsCommandList* GetGraphicsCommandList() const
	{
		return AccessInterface ? AccessInterface->GetD3D12GraphicsCommandList() : nullptr;
	}
#endif

private:
	ID3D12RHIAccessModule* Module;
	ID3D12RHIAccessInterface* AccessInterface;
};
