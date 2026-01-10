#pragma once

#include "CoreMinimal.h"
#include "ID3D12RHIAccessModule.h"

#if WITH_D3D12_RHI
#include "D3D12RHI.h"
#include "D3D12Util.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

class FD3D12RHIAccess : public ID3D12RHIAccessInterface
{
public:
	FD3D12RHIAccess();
	virtual ~FD3D12RHIAccess();

#if WITH_D3D12_RHI
	virtual ID3D12Device* GetD3D12Device() override;
	virtual ID3D12CommandQueue* GetD3D12CommandQueue() override;
	virtual ID3D12GraphicsCommandList* GetD3D12GraphicsCommandList() override;

private:
	ID3D12Device* CachedDevice;
	ID3D12CommandQueue* CachedCommandQueue;
	
	void UpdateCachedReferences();
#endif
};

class FD3D12RHIAccessModule : public ID3D12RHIAccessModule
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** ID3D12RHIAccessModule implementation */
	virtual ID3D12RHIAccessInterface* GetD3D12RHIAccessInterface() override;
	virtual bool IsD3D12Available() const override;

private:
	TUniquePtr<FD3D12RHIAccess> D3D12Access;
};
