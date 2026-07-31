#include "NvidiaHBAOPlusRenderer.h"

#include "D3D12CommandContext.h"
#include "D3D12RHIAccessHelper.h"
#include "DynamicRHI.h"
#include "ID3D12DynamicRHI.h"
#include "NvidiaHBAOPlusAsset.h"
#include "RHICommandList.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphResources.h"
#include "RendererInterface.h"
#include "RenderingThread.h"

#if WITH_NVIDIA_HBAOPLUS
THIRD_PARTY_INCLUDES_START
#include "GFSDK_SSAO.h"
THIRD_PARTY_INCLUDES_END
#endif

DEFINE_LOG_CATEGORY_STATIC(LogNvidiaHBAOPlusRenderer, Log, All);

BEGIN_SHADER_PARAMETER_STRUCT(FNvidiaHBAOPlusPassParameters, )
RDG_TEXTURE_ACCESS(DepthTexture, ERHIAccess::SRVGraphics)
RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

namespace NvidiaHBAOPlusRendererLocals
{
#if WITH_NVIDIA_HBAOPLUS
constexpr uint32 AppDescriptorCount = GFSDK_SSAO_MAX_FRAMES_IN_FLIGHT_D3D12;

const TCHAR *StatusToString(GFSDK_SSAO_Status Status)
{
	switch (Status)
	{
	case GFSDK_SSAO_OK:
		return TEXT("OK");
	case GFSDK_SSAO_VERSION_MISMATCH:
		return TEXT("Version mismatch");
	case GFSDK_SSAO_NULL_ARGUMENT:
		return TEXT("Null argument");
	case GFSDK_SSAO_INVALID_PROJECTION_MATRIX:
		return TEXT("Invalid projection matrix");
	case GFSDK_SSAO_INVALID_VIEWPORT_DIMENSIONS:
		return TEXT("Invalid viewport dimensions");
	case GFSDK_SSAO_D3D_RESOURCE_CREATION_FAILED:
		return TEXT("D3D resource creation failed");
	case GFSDK_SSAO_D3D12_INVALID_HEAP_TYPE:
		return TEXT("Invalid descriptor heap type");
	case GFSDK_SSAO_D3D12_INSUFFICIENT_DESCRIPTORS:
		return TEXT("Insufficient descriptors");
	case GFSDK_SSAO_D3D12_INVALID_NODE_MASK:
		return TEXT("Invalid D3D12 node mask");
	case GFSDK_SSAO_D3D12_INVALID_FRAME_INDEX:
		return TEXT("Invalid frame index");
	default:
		return TEXT("Unknown error");
	}
}

DXGI_FORMAT GetDepthShaderResourceFormat(DXGI_FORMAT ResourceFormat)
{
	switch (ResourceFormat)
	{
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
		return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
		return DXGI_FORMAT_R32_FLOAT;
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_D16_UNORM:
		return DXGI_FORMAT_R16_UNORM;
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_R16_UNORM:
		return ResourceFormat;
	default:
		return DXGI_FORMAT_UNKNOWN;
	}
}

GFSDK_SSAO_Parameters MakeSdkParameters(
	const FNvidiaHBAOPlusRenderSettings &Settings)
{
	GFSDK_SSAO_Parameters Result;
	Result.Radius = Settings.Radius;
	Result.Bias = Settings.Bias;
	Result.SmallScaleAO = Settings.SmallScaleAO;
	Result.LargeScaleAO = Settings.LargeScaleAO;
	Result.PowerExponent = Settings.PowerExponent;
	Result.StepCount = Settings.StepCount == 8 ? GFSDK_SSAO_STEP_COUNT_8
											   : GFSDK_SSAO_STEP_COUNT_4;
	Result.DepthStorage = Settings.DepthStorage == 32
							  ? GFSDK_SSAO_FP32_VIEW_DEPTHS
							  : GFSDK_SSAO_FP16_VIEW_DEPTHS;
	Result.DepthClampMode = GFSDK_SSAO_CLAMP_TO_EDGE;
	Result.Blur.Enable = Settings.bEnableBlur;
	Result.Blur.Radius = Settings.BlurRadius == 2 ? GFSDK_SSAO_BLUR_RADIUS_2
												  : GFSDK_SSAO_BLUR_RADIUS_4;
	Result.Blur.Sharpness = Settings.BlurSharpness;
	Result.EnableDualLayerAO = false;
	return Result;
}

FORCENOINLINE void InvalidateD3D12GraphicsState(FD3D12ContextCommon &Context)
{
	// HBAO+ records native graphics PSOs, root signatures, descriptor
	// heaps, viewports, and bindings outside UE's state cache. Clear the
	// cache so the next UE draw rebinds every required graphics state.
	Context.ClearState();
}
#endif
} // namespace NvidiaHBAOPlusRendererLocals

struct FNvidiaHBAOPlusRenderer::FImpl
{
#if WITH_NVIDIA_HBAOPLUS
	struct FViewContext
	{
		~FViewContext()
		{
			if (Context != nullptr)
			{
				Context->Release();
				Context = nullptr;
			}
			if (RtvHeap != nullptr)
			{
				RtvHeap->Release();
				RtvHeap = nullptr;
			}
			if (ShaderHeap != nullptr)
			{
				ShaderHeap->Release();
				ShaderHeap = nullptr;
			}
		}

		bool Initialize(ID3D12Device *Device, uint32 NodeMask,
						FString &OutError)
		{
			D3D12_DESCRIPTOR_HEAP_DESC ShaderHeapDesc = {};
			ShaderHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			ShaderHeapDesc.NumDescriptors =
				NvidiaHBAOPlusRendererLocals::AppDescriptorCount +
				GFSDK_SSAO_NUM_DESCRIPTORS_CBV_SRV_UAV_HEAP_D3D12;
			ShaderHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			ShaderHeapDesc.NodeMask = NodeMask;

			HRESULT Result = Device->CreateDescriptorHeap(
				&ShaderHeapDesc, IID_PPV_ARGS(&ShaderHeap));
			if (FAILED(Result))
			{
				OutError = FString::Printf(
					TEXT("Create shader descriptor heap failed (0x%08X)"),
					static_cast<uint32>(Result));
				return false;
			}

			D3D12_DESCRIPTOR_HEAP_DESC RtvHeapDesc = {};
			RtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			RtvHeapDesc.NumDescriptors =
				GFSDK_SSAO_NUM_DESCRIPTORS_RTV_HEAP_D3D12;
			RtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			RtvHeapDesc.NodeMask = NodeMask;

			Result = Device->CreateDescriptorHeap(&RtvHeapDesc,
												  IID_PPV_ARGS(&RtvHeap));
			if (FAILED(Result))
			{
				OutError = FString::Printf(
					TEXT("Create RTV descriptor heap failed (0x%08X)"),
					static_cast<uint32>(Result));
				return false;
			}

			GFSDK_SSAO_DescriptorHeaps_D3D12 DescriptorHeaps;
			DescriptorHeaps.CBV_SRV_UAV.pDescHeap = ShaderHeap;
			DescriptorHeaps.CBV_SRV_UAV.BaseIndex =
				NvidiaHBAOPlusRendererLocals::AppDescriptorCount;
			DescriptorHeaps.RTV.pDescHeap = RtvHeap;
			DescriptorHeaps.RTV.BaseIndex = 0;

			GFSDK_SSAO_CustomHeap CustomHeap;
			CustomHeap.new_ = ::operator new;
			CustomHeap.delete_ = ::operator delete;

			const GFSDK_SSAO_Status CreateStatus =
				GFSDK_SSAO_CreateContext_D3D12(
					Device, NodeMask, DescriptorHeaps, &Context, &CustomHeap);
			if (CreateStatus != GFSDK_SSAO_OK || Context == nullptr)
			{
				OutError = FString::Printf(
					TEXT("GFSDK_SSAO_CreateContext_D3D12 failed: %s (%d)"),
					NvidiaHBAOPlusRendererLocals::StatusToString(CreateStatus),
					static_cast<int32>(CreateStatus));
				return false;
			}

			return true;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthCpuHandle(ID3D12Device *Device,
													  uint32 FrameIndex) const
		{
			D3D12_CPU_DESCRIPTOR_HANDLE Handle =
				ShaderHeap->GetCPUDescriptorHandleForHeapStart();
			Handle.ptr += static_cast<SIZE_T>(FrameIndex) *
						  Device->GetDescriptorHandleIncrementSize(
							  D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			return Handle;
		}

		D3D12_GPU_DESCRIPTOR_HANDLE GetDepthGpuHandle(ID3D12Device *Device,
													  uint32 FrameIndex) const
		{
			D3D12_GPU_DESCRIPTOR_HANDLE Handle =
				ShaderHeap->GetGPUDescriptorHandleForHeapStart();
			Handle.ptr += static_cast<UINT64>(FrameIndex) *
						  Device->GetDescriptorHandleIncrementSize(
							  D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			return Handle;
		}

		GFSDK_SSAO_Context_D3D12 *Context = nullptr;
		ID3D12DescriptorHeap *ShaderHeap = nullptr;
		ID3D12DescriptorHeap *RtvHeap = nullptr;
		bool bLoggedFirstSuccessfulPass = false;
	};

	TMap<uint32, TUniquePtr<FViewContext>> ViewContexts;
	ID3D12Device *Device = nullptr;
	uint32 ViewSlotFrameNumber = MAX_uint32;
	uint32 NextViewSlot = 0;
#endif
};

FNvidiaHBAOPlusRenderSettings FNvidiaHBAOPlusRenderSettings::FromAsset(
	const UNvidiaHBAOPlusAsset &Asset)
{
	FNvidiaHBAOPlusRenderSettings Result;
	Result.bEnabled = Asset.bEnabled;
	Result.bVisualizeAO = Asset.bVisualizeAO;
	Result.Radius = Asset.Radius;
	Result.Bias = Asset.Bias;
	Result.SmallScaleAO = Asset.SmallScaleAO;
	Result.LargeScaleAO = Asset.LargeScaleAO;
	Result.PowerExponent = Asset.PowerExponent;
	Result.StepCount =
		Asset.StepCount == ENvidiaHBAOPlusStepCount::Steps8 ? 8 : 4;
	Result.DepthStorage =
		Asset.DepthStorage == ENvidiaHBAOPlusDepthStorage::FP32 ? 32 : 16;
	Result.bEnableBlur = Asset.bEnableBlur;
	Result.BlurRadius =
		Asset.BlurRadius == ENvidiaHBAOPlusBlurRadius::Radius2 ? 2 : 4;
	Result.BlurSharpness = Asset.BlurSharpness;
	return Result;
}

FNvidiaHBAOPlusRenderer::FNvidiaHBAOPlusRenderer() : Impl(MakeUnique<FImpl>())
{
}

FNvidiaHBAOPlusRenderer::~FNvidiaHBAOPlusRenderer()
{
#if WITH_NVIDIA_HBAOPLUS
	ensureMsgf(Impl == nullptr || Impl->ViewContexts.IsEmpty(),
			   TEXT("HBAO+ renderer was destroyed without Shutdown()"));
#endif
}

void FNvidiaHBAOPlusRenderer::UpdateSettings(
	const FNvidiaHBAOPlusRenderSettings &InSettings)
{
	FScopeLock Lock(&SettingsMutex);
	Settings = InSettings;
}

void FNvidiaHBAOPlusRenderer::AddPostOpaquePass(
	FPostOpaqueRenderParameters &Parameters)
{
	if (bShutdown.Load())
	{
		return;
	}

	FNvidiaHBAOPlusRenderSettings SettingsSnapshot;
	{
		FScopeLock Lock(&SettingsMutex);
		SettingsSnapshot = Settings;
	}

	if (!SettingsSnapshot.bEnabled)
	{
		SetStatus(TEXT("Disabled"));
		return;
	}

	if (Parameters.GraphBuilder == nullptr ||
		Parameters.ColorTexture == nullptr ||
		Parameters.DepthTexture == nullptr || Parameters.ViewportRect.IsEmpty())
	{
		SetStatus(TEXT("Waiting for valid scene color and depth"));
		return;
	}

	if (GDynamicRHI == nullptr ||
		GDynamicRHI->GetInterfaceType() != ERHIInterfaceType::D3D12)
	{
		SetStatus(TEXT("Requires the D3D12 RHI"));
		return;
	}

#if !WITH_NVIDIA_HBAOPLUS
	SetStatus(TEXT("HBAO+ SDK is unavailable for this platform"));
#else
	FRDGBuilder &GraphBuilder = *Parameters.GraphBuilder;
	FRDGTextureRef ColorTexture = Parameters.ColorTexture;
	FRDGTextureRef DepthTexture = Parameters.DepthTexture;
	const FIntRect ViewportRect = Parameters.ViewportRect;
	const FMatrix ProjectionMatrix = Parameters.ProjMatrix;
	if (Impl->ViewSlotFrameNumber != GFrameNumberRenderThread)
	{
		Impl->ViewSlotFrameNumber = GFrameNumberRenderThread;
		Impl->NextViewSlot = 0;
	}
	const uint32 ViewSlot = Impl->NextViewSlot++;

	FNvidiaHBAOPlusPassParameters *PassParameters =
		GraphBuilder.AllocParameters<FNvidiaHBAOPlusPassParameters>();
	PassParameters->DepthTexture = DepthTexture;
	PassParameters->RenderTargets[0] =
		FRenderTargetBinding(ColorTexture, ERenderTargetLoadAction::ELoad);

	GraphBuilder.AddPass(
		RDG_EVENT_NAME("NVIDIA HBAO+ %dx%d", ViewportRect.Width(),
					   ViewportRect.Height()),
		PassParameters,
		ERDGPassFlags::Raster | ERDGPassFlags::NeverCull |
			ERDGPassFlags::NeverParallel,
		[this, SettingsSnapshot, ColorTexture, DepthTexture, ViewportRect,
		 ProjectionMatrix, ViewSlot](FRHICommandListImmediate &RHICmdList)
		{
			if (bShutdown.Load())
			{
				return;
			}

			DepthTexture->MarkResourceAsUsed();
			ColorTexture->MarkResourceAsUsed();
			FTextureRHIRef DepthTextureRHI = DepthTexture->GetRHI();
			FTextureRHIRef ColorTextureRHI = ColorTexture->GetRHI();
			const uint32 FrameIndex =
				static_cast<uint32>(GFrameNumberRenderThread) %
				GFSDK_SSAO_MAX_FRAMES_IN_FLIGHT_D3D12;

			RHICmdList.EnqueueLambda(
				[this, SettingsSnapshot,
				 DepthTextureRHI = MoveTemp(DepthTextureRHI),
				 ColorTextureRHI = MoveTemp(ColorTextureRHI), ViewportRect,
				 ProjectionMatrix, ViewSlot,
				 FrameIndex](FRHICommandListBase &ExecutingCmdList)
				{
					if (bShutdown.Load())
					{
						return;
					}

					FD3D12RHIAccessHelper D3D12Access;
					if (!D3D12Access.IsAvailable())
					{
						SetStatus(TEXT("D3D12RHIAccess is unavailable"));
						return;
					}

					ID3D12Device *Device = D3D12Access.GetDevice();
					ID3D12CommandQueue *CommandQueue =
						D3D12Access.GetCommandQueue();
					ID3D12GraphicsCommandList *CommandList =
						D3D12Access.GetGraphicsCommandList(ExecutingCmdList);
					ID3D12DynamicRHI *D3D12RHI = GetID3D12DynamicRHI();
					if (Device == nullptr || CommandQueue == nullptr ||
						CommandList == nullptr || D3D12RHI == nullptr)
					{
						SetStatus(
							TEXT("UE did not expose the active D3D12 objects"));
						return;
					}

					if (Impl->Device != nullptr && Impl->Device != Device)
					{
						SetStatus(TEXT(
							"D3D12 device changed; reset the HBAO+ context"));
						return;
					}
					Impl->Device = Device;

					TUniquePtr<FImpl::FViewContext> &ViewContext =
						Impl->ViewContexts.FindOrAdd(ViewSlot);
					if (!ViewContext)
					{
						ViewContext = MakeUnique<FImpl::FViewContext>();
						FString CreateError;
						const uint32 NodeMask =
							D3D12RHI->RHIGetDeviceNodeMask(0);
						if (!ViewContext->Initialize(Device, NodeMask,
													 CreateError))
						{
							SetStatus(CreateError);
							ViewContext.Reset();
							return;
						}
					}

					ID3D12Resource *DepthResource =
						D3D12RHI->RHIGetResource(DepthTextureRHI);
					ID3D12Resource *ColorResource =
						D3D12RHI->RHIGetResource(ColorTextureRHI);
					if (DepthResource == nullptr || ColorResource == nullptr)
					{
						SetStatus(
							TEXT("Failed to resolve native scene textures"));
						return;
					}

					const D3D12_RESOURCE_DESC DepthDesc =
						DepthResource->GetDesc();
					const DXGI_FORMAT DepthSrvFormat =
						NvidiaHBAOPlusRendererLocals::
							GetDepthShaderResourceFormat(DepthDesc.Format);
					if (DepthSrvFormat == DXGI_FORMAT_UNKNOWN)
					{
						SetStatus(FString::Printf(
							TEXT("Unsupported scene-depth DXGI format (%d)"),
							static_cast<int32>(DepthDesc.Format)));
						return;
					}

					if (ViewportRect.Min.X < 0 || ViewportRect.Min.Y < 0 ||
						ViewportRect.Max.X >
							static_cast<int32>(DepthDesc.Width) ||
						ViewportRect.Max.Y >
							static_cast<int32>(DepthDesc.Height))
					{
						SetStatus(
							TEXT("View rectangle exceeds the depth texture"));
						return;
					}

					D3D12_SHADER_RESOURCE_VIEW_DESC DepthSrvDesc = {};
					DepthSrvDesc.Format = DepthSrvFormat;
					DepthSrvDesc.Shader4ComponentMapping =
						D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					if (DepthDesc.SampleDesc.Count > 1)
					{
						DepthSrvDesc.ViewDimension =
							D3D12_SRV_DIMENSION_TEXTURE2DMS;
					}
					else
					{
						DepthSrvDesc.ViewDimension =
							D3D12_SRV_DIMENSION_TEXTURE2D;
						DepthSrvDesc.Texture2D.MostDetailedMip = 0;
						DepthSrvDesc.Texture2D.MipLevels = 1;
						DepthSrvDesc.Texture2D.PlaneSlice = 0;
						DepthSrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
					}

					const D3D12_CPU_DESCRIPTOR_HANDLE DepthCpuHandle =
						ViewContext->GetDepthCpuHandle(Device, FrameIndex);
					const D3D12_GPU_DESCRIPTOR_HANDLE DepthGpuHandle =
						ViewContext->GetDepthGpuHandle(Device, FrameIndex);
					Device->CreateShaderResourceView(
						DepthResource, &DepthSrvDesc, DepthCpuHandle);

					GFSDK_SSAO_InputData_D3D12 InputData;
					InputData.DepthData.DepthTextureType =
						GFSDK_SSAO_HARDWARE_DEPTHS;
					InputData.DepthData.FullResDepthTextureSRV.pResource =
						DepthResource;
					InputData.DepthData.FullResDepthTextureSRV.GpuHandle =
						DepthGpuHandle.ptr;
					const FMatrix44f ProjectionMatrixFloat(ProjectionMatrix);
					InputData.DepthData.ProjectionMatrix.Data =
						GFSDK_SSAO_Float4x4(&ProjectionMatrixFloat.M[0][0]);
					InputData.DepthData.ProjectionMatrix.Layout =
						GFSDK_SSAO_ROW_MAJOR_ORDER;
					InputData.DepthData.MetersToViewSpaceUnits = 100.0f;
					InputData.DepthData.Viewport.Enable = true;
					InputData.DepthData.Viewport.TopLeftX = ViewportRect.Min.X;
					InputData.DepthData.Viewport.TopLeftY = ViewportRect.Min.Y;
					InputData.DepthData.Viewport.Width = ViewportRect.Width();
					InputData.DepthData.Viewport.Height = ViewportRect.Height();
					InputData.DepthData.Viewport.MinDepth = 0.0f;
					InputData.DepthData.Viewport.MaxDepth = 1.0f;
					InputData.NormalData.Enable = false;

					GFSDK_SSAO_RenderTargetView_D3D12 OutputRtv;
					OutputRtv.pResource = ColorResource;
					OutputRtv.CpuHandle =
						D3D12RHI->RHIGetRenderTargetView(ColorTextureRHI).ptr;

					GFSDK_SSAO_Output_D3D12 Output;
					Output.pRenderTargetView = &OutputRtv;
					Output.Blend.Mode = SettingsSnapshot.bVisualizeAO
											? GFSDK_SSAO_OVERWRITE_RGB
											: GFSDK_SSAO_MULTIPLY_RGB;

					// RDG has transitioned scene color to RTV, but D3D12RHI may
					// defer emitting the native barrier until UE's next draw.
					// HBAO+ draws directly, so flush that pending barrier first.
					D3D12RHI->RHIFlushResourceBarriers(ExecutingCmdList, 0);

					ID3D12DescriptorHeap *DescriptorHeaps[] = {
						ViewContext->ShaderHeap};
					CommandList->SetDescriptorHeaps(
						UE_ARRAY_COUNT(DescriptorHeaps), DescriptorHeaps);

					const GFSDK_SSAO_Parameters SdkParameters =
						NvidiaHBAOPlusRendererLocals::MakeSdkParameters(
							SettingsSnapshot);
					const GFSDK_SSAO_Status RenderStatus =
						ViewContext->Context->RenderAOMultiFrame(
							CommandQueue, CommandList, InputData, SdkParameters,
							Output, GFSDK_SSAO_RENDER_AO, FrameIndex);

					D3D12RHI->RHIFinishExternalComputeWork(ExecutingCmdList, 0,
														   CommandList);
					NvidiaHBAOPlusRendererLocals::InvalidateD3D12GraphicsState(
						static_cast<FD3D12ContextCommon &>(
							FD3D12CommandContext::Get(ExecutingCmdList, 0)));

					if (RenderStatus != GFSDK_SSAO_OK)
					{
						SetStatus(FString::Printf(
							TEXT("RenderAOMultiFrame failed: %s (%d)"),
							NvidiaHBAOPlusRendererLocals::StatusToString(
								RenderStatus),
							static_cast<int32>(RenderStatus)));
						return;
					}

					if (!ViewContext->bLoggedFirstSuccessfulPass)
					{
						UE_LOG(
							LogNvidiaHBAOPlusRenderer, Log,
							TEXT("HBAO+ pass is running at %dx%d (%d steps)"),
							ViewportRect.Width(), ViewportRect.Height(),
							SettingsSnapshot.StepCount);
						ViewContext->bLoggedFirstSuccessfulPass = true;
					}

					SetStatus(FString::Printf(
						TEXT("Running | %dx%d | %d steps | %.1f MB"),
						ViewportRect.Width(), ViewportRect.Height(),
						SettingsSnapshot.StepCount,
						static_cast<double>(
							ViewContext->Context
								->GetAllocatedVideoMemoryBytes()) /
							(1024.0 * 1024.0)));
				});
		});
#endif
}

void FNvidiaHBAOPlusRenderer::RequestContextReset()
{
	if (bShutdown.Load())
	{
		return;
	}

	SetStatus(TEXT("Resetting HBAO+ contexts"));
	ENQUEUE_RENDER_COMMAND(NvidiaHBAOPlusResetContexts)(
		[this](FRHICommandListImmediate &RHICmdList)
		{
			if (!bShutdown.Load() && GDynamicRHI != nullptr)
			{
				RHICmdList.SubmitAndBlockUntilGPUIdle();
				ReleaseContexts_RenderThread();
				SetStatus(TEXT("Contexts reset; waiting for a rendered view"));
			}
		});
}

void FNvidiaHBAOPlusRenderer::Shutdown()
{
	if (bShutdown.Exchange(true))
	{
		return;
	}

	if (IsInRenderingThread())
	{
		FRHICommandListImmediate &RHICmdList = FRHICommandListImmediate::Get();
		if (GDynamicRHI != nullptr)
		{
			RHICmdList.SubmitAndBlockUntilGPUIdle();
		}
		ReleaseContexts_RenderThread();
		return;
	}

	ENQUEUE_RENDER_COMMAND(NvidiaHBAOPlusShutdown)(
		[this](FRHICommandListImmediate &RHICmdList)
		{
			if (GDynamicRHI != nullptr)
			{
				RHICmdList.SubmitAndBlockUntilGPUIdle();
			}
			ReleaseContexts_RenderThread();
		});
	FlushRenderingCommands();
	SetStatus(TEXT("Stopped"));
}

FString FNvidiaHBAOPlusRenderer::GetStatus() const
{
	FScopeLock Lock(&StatusMutex);
	return Status;
}

void FNvidiaHBAOPlusRenderer::ReleaseContexts_RenderThread()
{
	check(IsInRenderingThread());
#if WITH_NVIDIA_HBAOPLUS
	Impl->ViewContexts.Reset();
	Impl->Device = nullptr;
	Impl->ViewSlotFrameNumber = MAX_uint32;
	Impl->NextViewSlot = 0;
#endif
}

void FNvidiaHBAOPlusRenderer::SetStatus(const FString &InStatus)
{
	FScopeLock Lock(&StatusMutex);
	Status = InStatus;
}
