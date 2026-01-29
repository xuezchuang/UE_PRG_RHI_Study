#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#if WITH_D3D12_RHI
struct ID3D12Device;
struct ID3D12CommandQueue;
struct ID3D12GraphicsCommandList;
#endif

/**
 * Interface for accessing D3D12 RHI native objects
 * This interface allows plugins to get access to native D3D12 objects
 * for integration with NVIDIA SDKs and other D3D12-based libraries
 */
class ID3D12RHIAccessInterface
{
public:
	virtual ~ID3D12RHIAccessInterface() {}

#if WITH_D3D12_RHI
	/**
	 * Get the native D3D12 device
	 * @return Pointer to ID3D12Device or nullptr if not available
	 */
	virtual ID3D12Device* GetD3D12Device() = 0;

	/**
	 * Get the native D3D12 command queue for graphics operations
	 * @return Pointer to ID3D12CommandQueue or nullptr if not available
	 */
	virtual ID3D12CommandQueue* GetD3D12CommandQueue() = 0;

	/**
	 * Get the current native D3D12 graphics command list
	 * @return Pointer to ID3D12GraphicsCommandList or nullptr if not available
	 */
	virtual ID3D12GraphicsCommandList* GetD3D12GraphicsCommandList() = 0;
#endif
};

/**
 * Module interface for D3D12RHIAccess plugin
 */
class ID3D12RHIAccessModule : public IModuleInterface
{
public:
	/**
	 * Get the D3D12 RHI Access interface
	 * @return Pointer to ID3D12RHIAccessInterface or nullptr if not available
	 */
	virtual ID3D12RHIAccessInterface* GetD3D12RHIAccessInterface() = 0;

	/**
	 * Check if D3D12 RHI is available on this platform
	 * @return true if D3D12 is available, false otherwise
	 */
	virtual bool IsD3D12Available() const = 0;
};
