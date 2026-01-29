#pragma once

#include "CoreMinimal.h"

/**
 * Example class demonstrating how to use D3D12 RHI Access for NVIDIA SDK integration
 * This can be used as a reference for implementing NVIDIA WaveWorks or other D3D12-based features
 */
class FD3D12IntegrationExample
{
public:
	FD3D12IntegrationExample();
	~FD3D12IntegrationExample();

	/**
	 * Initialize D3D12 integration
	 * @return true if initialization succeeded
	 */
	bool Initialize();

	/**
	 * Shutdown and cleanup D3D12 resources
	 */
	void Shutdown();

	/**
	 * Check if D3D12 integration is available and initialized
	 */
	bool IsInitialized() const { return bIsInitialized; }

	/**
	 * Example function showing how to access D3D12 device
	 * In a real implementation, this would use NVIDIA SDK functions
	 */
	void ExampleD3D12Usage();

private:
	bool bIsInitialized;
};
