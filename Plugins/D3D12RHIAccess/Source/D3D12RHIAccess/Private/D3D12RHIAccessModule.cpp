#include "D3D12RHIAccessModule.h"
#include "Modules/ModuleManager.h"
#include "RHI.h"
#include "DynamicRHI.h"

#if WITH_D3D12_RHI
#include "D3D12RHI.h"
#include "D3D12RHIPrivate.h"
#endif

#define LOCTEXT_NAMESPACE "FD3D12RHIAccessModule"

DEFINE_LOG_CATEGORY_STATIC(LogD3D12RHIAccess, Log, All);

//////////////////////////////////////////////////////////////////////////
// FD3D12RHIAccess Implementation
//////////////////////////////////////////////////////////////////////////

FD3D12RHIAccess::FD3D12RHIAccess()
#if WITH_D3D12_RHI
	: CachedDevice(nullptr)
	, CachedCommandQueue(nullptr)
#endif
{
#if WITH_D3D12_RHI
	UpdateCachedReferences();
#endif
}

FD3D12RHIAccess::~FD3D12RHIAccess()
{
	// Note: We don't release these references as they are owned by the RHI
}

#if WITH_D3D12_RHI
void FD3D12RHIAccess::UpdateCachedReferences()
{
	CachedDevice = nullptr;
	CachedCommandQueue = nullptr;

	// Get the dynamic RHI
	FDynamicRHI* DynamicRHI = GDynamicRHI;
	if (!DynamicRHI)
	{
		UE_LOG(LogD3D12RHIAccess, Warning, TEXT("DynamicRHI is not available"));
		return;
	}

	// Check if we're using D3D12
	const TCHAR* RHIName = GDynamicRHI->GetName();
	if (FCString::Strcmp(RHIName, TEXT("D3D12")) != 0)
	{
		UE_LOG(LogD3D12RHIAccess, Warning, TEXT("Current RHI is %s, not D3D12"), RHIName);
		return;
	}

	// Get D3D12 device from the RHI
	ID3D12DynamicRHI* D3D12RHI = GetID3D12DynamicRHI();
	if (D3D12RHI)
	{
		CachedDevice = D3D12RHI->RHIGetDevice(0); // Get default adapter device
		CachedCommandQueue = D3D12RHI->RHIGetCommandQueue();
		
		if (CachedDevice)
		{
			UE_LOG(LogD3D12RHIAccess, Log, TEXT("Successfully obtained D3D12 Device"));
		}
		if (CachedCommandQueue)
		{
			UE_LOG(LogD3D12RHIAccess, Log, TEXT("Successfully obtained D3D12 Command Queue"));
		}
	}
	else
	{
		UE_LOG(LogD3D12RHIAccess, Error, TEXT("Failed to get ID3D12DynamicRHI interface"));
	}
}

ID3D12Device* FD3D12RHIAccess::GetD3D12Device()
{
	if (!CachedDevice)
	{
		UpdateCachedReferences();
	}
	return CachedDevice;
}

ID3D12CommandQueue* FD3D12RHIAccess::GetD3D12CommandQueue()
{
	if (!CachedCommandQueue)
	{
		UpdateCachedReferences();
	}
	return CachedCommandQueue;
}

ID3D12GraphicsCommandList* FD3D12RHIAccess::GetD3D12GraphicsCommandList()
{
	// Note: Command lists are typically created per-frame and are not cached
	// This method returns the current command list from the RHI if available
	ID3D12DynamicRHI* D3D12RHI = GetID3D12DynamicRHI();
	if (D3D12RHI)
	{
		// In UE5, the command list is typically accessed through the command context
		// This is a simplified implementation - in practice, you'd need to access
		// the current command context to get the active command list
		return D3D12RHI->RHIGetGraphicsCommandList(0);
	}
	return nullptr;
}
#endif

//////////////////////////////////////////////////////////////////////////
// FD3D12RHIAccessModule Implementation
//////////////////////////////////////////////////////////////////////////

void FD3D12RHIAccessModule::StartupModule()
{
	UE_LOG(LogD3D12RHIAccess, Log, TEXT("D3D12RHIAccess module starting up"));
	
#if WITH_D3D12_RHI
	// Create the D3D12 access interface
	D3D12Access = MakeUnique<FD3D12RHIAccess>();
	UE_LOG(LogD3D12RHIAccess, Log, TEXT("D3D12 RHI Access interface created"));
#else
	UE_LOG(LogD3D12RHIAccess, Warning, TEXT("D3D12 RHI is not available on this platform"));
#endif
}

void FD3D12RHIAccessModule::ShutdownModule()
{
	UE_LOG(LogD3D12RHIAccess, Log, TEXT("D3D12RHIAccess module shutting down"));
	D3D12Access.Reset();
}

ID3D12RHIAccessInterface* FD3D12RHIAccessModule::GetD3D12RHIAccessInterface()
{
#if WITH_D3D12_RHI
	return D3D12Access.Get();
#else
	return nullptr;
#endif
}

bool FD3D12RHIAccessModule::IsD3D12Available() const
{
#if WITH_D3D12_RHI
	if (GDynamicRHI)
	{
		const TCHAR* RHIName = GDynamicRHI->GetName();
		return FCString::Strcmp(RHIName, TEXT("D3D12")) == 0;
	}
#endif
	return false;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FD3D12RHIAccessModule, D3D12RHIAccess)
