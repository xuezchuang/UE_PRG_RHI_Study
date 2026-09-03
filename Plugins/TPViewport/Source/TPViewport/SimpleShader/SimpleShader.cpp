#include "SimpleShader.h"

#include "Containers/DynamicRHIResourceArray.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "Engine/TextureRenderTarget2D.h"

#include "RHIStaticStates.h"
#include "PipelineStateCache.h"

#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "ImageUtils.h"

#include "RenderResource.h"
#include "GlobalRenderResources.h"
#include "RHI.h"
#include "RenderCommandFence.h"
#include "Misc/FileHelper.h"

IMPLEMENT_SHADER_TYPE(, FSimpleShaderVS, TEXT("/Plugin/TPViewport/Private/SimpleShader.usf"), TEXT("MainVS"), SF_Vertex)
IMPLEMENT_SHADER_TYPE(, FSimpleShaderPS, TEXT("/Plugin/TPViewport/Private/SimpleShader.usf"), TEXT("MainPS"), SF_Pixel)

struct FColorVertex
{
	FVector4f Position;
	FVector4f Color;
};

class FSimpleVertexBuffer : public FVertexBuffer
{
public:
	void InitRHI() override
	{
		TResourceArray<FColorVertex, VERTEXBUFFER_ALIGNMENT> Vertices;
		Vertices.SetNumUninitialized(3);

		Vertices[0].Position = FVector4f(-0.5f, -0.5f, 0.0f, 1.0f);
		Vertices[0].Color = FVector4f(1.0f, 0.0f, 0.0f, 1.0f);
		Vertices[1].Position = FVector4f(0.5f, -0.5f, 0.0f, 1.0f);
		Vertices[1].Color = FVector4f(0.0f, 1.0f, 0.0f, 1.0f);
		Vertices[2].Position = FVector4f(0.0f, 0.5f, 0.0f, 1.0f);
		Vertices[2].Color = FVector4f(0.0f, 0.0f, 1.0f, 1.0f);

		FRHIResourceCreateInfo CreateInfo(TEXT("FSimpleVertexBuffer"), &Vertices);
		VertexBufferRHI = RHICreateVertexBuffer(Vertices.GetResourceDataSize(), BUF_Static | BUF_VertexBuffer, CreateInfo);
	}
};

TGlobalResource<FSimpleVertexBuffer> GSimpleVertexBuffer;

class FColorVertexDeclaration : public FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;

	virtual void InitRHI() override
	{
		FVertexDeclarationElementList Elements;
		uint16 Stride = sizeof(FColorVertex);
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FColorVertex, Position), VET_Float4, 0, Stride));
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FColorVertex, Color), VET_Float4, 1, Stride));
		VertexDeclarationRHI = PipelineStateCache::GetOrCreateVertexDeclaration(Elements);
	}

	virtual void ReleaseRHI() override
	{
		VertexDeclarationRHI.SafeRelease();
	}
};

TGlobalResource<FColorVertexDeclaration> GSimpleVertexDeclaration;

class FSimpleIndexBuffer : public FIndexBuffer
{
public:
	void InitRHI() override
	{
		const uint16 Indices[] = { 0, 1, 2 };

		TResourceArray<uint16, INDEXBUFFER_ALIGNMENT> IndexBuffer;
		const uint32 NumIndices = UE_ARRAY_COUNT(Indices);
		IndexBuffer.AddUninitialized(NumIndices);
		FMemory::Memcpy(IndexBuffer.GetData(), Indices, NumIndices * sizeof(uint16));

		FRHIResourceCreateInfo CreateInfo(TEXT("FSimpleIndexBuffer"));
		CreateInfo.ResourceArray = &IndexBuffer;

		IndexBufferRHI = RHICreateIndexBuffer(sizeof(uint16), IndexBuffer.GetResourceDataSize(), BUF_Static, CreateInfo);
	}
};

TGlobalResource<FSimpleIndexBuffer> GSimpleIndexBuffer;


// ------------------------------
// RenderThread draw
// ------------------------------
static void DrawTestShaderRenderTarget_RenderThread(
	FRHICommandListImmediate& RHICmdList,
	FTextureRenderTargetResource* OutputRenderTargetResource,
	int32 nType,
	FTextureRHIRef InputTextureRHI
)
{
	check(IsInRenderingThread());
	FRHITexture* ShaderTexture = InputTextureRHI.IsValid()
		? InputTextureRHI.GetReference()
		: GWhiteTexture->TextureRHI.GetReference();
	check(ShaderTexture);

	FRHITexture2D* RenderTargetTexture = OutputRenderTargetResource->GetRenderTargetTexture();

	FRHIRenderPassInfo RPInfo(RenderTargetTexture, ERenderTargetActions::DontLoad_Store);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("SimpleRendererShaderPass"));
	{
		const FIntPoint RTSize(OutputRenderTargetResource->GetSizeX(), OutputRenderTargetResource->GetSizeY());

		RHICmdList.SetViewport(0, 0, 0.f, RTSize.X, RTSize.Y, 1.f);

		// Shaders
		FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
		TShaderMapRef<FSimpleShaderVS> VertexShader(GlobalShaderMap);
		TShaderMapRef<FSimpleShaderPS> PixelShader(GlobalShaderMap);

		// PSO
		FGraphicsPipelineStateInitializer GraphicsPSOInit;
		RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
		GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
		GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
		GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
		GraphicsPSOInit.PrimitiveType = PT_TriangleList;
		GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GSimpleVertexDeclaration.VertexDeclarationRHI;
		GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
		GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
		SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);

		// Texture2D interface -> FShaderResourceParameter -> SetShaderTexture -> D3D12 SRV descriptor.
		VertexShader->SetParameters(RHICmdList, VertexShader.GetVertexShader(), nType);
		PixelShader->SetParameters(RHICmdList, PixelShader.GetPixelShader(), nType, ShaderTexture);

		RHICmdList.SetStreamSource(0, GSimpleVertexBuffer.VertexBufferRHI, 0);

		RHICmdList.DrawIndexedPrimitive(
			GSimpleIndexBuffer.IndexBufferRHI,
			/*BaseVertexIndex=*/0,
			/*MinIndex=*/0,
			/*NumVertices=*/3,
			/*StartIndex=*/0,
			/*NumPrimitives=*/1,
			/*NumInstances=*/1
		);
	}
	RHICmdList.EndRenderPass();
}


// ------------------------------
// GameThread API
// ------------------------------
void FSimpleRenderer::Render(UTextureRenderTarget2D* RenderTarget, TFunction<void()> OnRenderCompleted)
{
	Render(RenderTarget, nullptr, MoveTemp(OnRenderCompleted));
}

void FSimpleRenderer::Render(UTextureRenderTarget2D* RenderTarget, UTexture2D* InputTexture, TFunction<void()> OnRenderCompleted)
{
	check(IsInGameThread());

	if (!RenderTarget)
	{
		return;
	}

	FTextureRenderTargetResource* TextureRenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	FTextureRHIRef InputTextureRHI;
	if (InputTexture)
	{
		if (FTextureResource* InputTextureResource = InputTexture->GetResource())
		{
			InputTextureRHI = InputTextureResource->TextureRHI;
		}
	}

	ENQUEUE_RENDER_COMMAND(CaptureCommand)(
		[TextureRenderTargetResource, InputTextureRHI, OnRenderCompleted](FRHICommandListImmediate& RHICmdList)
		{
			// 你在这里改 nType：0 顶点色 / 1 纹理采样 / 2 testColor
			DrawTestShaderRenderTarget_RenderThread(RHICmdList, TextureRenderTargetResource, 1, InputTextureRHI);

			if (OnRenderCompleted)
			{
				OnRenderCompleted();
			}
		}
		);
}

void FSimpleRenderer::UpdateTextureRegion(
	UTextureRenderTarget2D* RenderTarget,
	int32 MipIndex,
	uint32 NumRegions,
	FUpdateTextureRegion2D Region,
	uint32 SrcPitch,
	uint32 SrcBpp,
	uint8* SrcData,
	TFunction<void(uint8* SrcData)> DataCleanupFunc
)
{
	check(IsInGameThread());
	FTextureRenderTargetResource* TextureRenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	ENQUEUE_RENDER_COMMAND(UpdateTextureRegionsData)(
		[=](FRHICommandListImmediate& RHICmdList)
		{
			FRHITexture2D* TextureRHI = TextureRenderTargetResource->GetTexture2DRHI();
			if (TextureRHI && TextureRHI->IsValid())
			{
				RHIUpdateTexture2D(
					TextureRHI,
					MipIndex,
					Region,
					SrcPitch,
					SrcData
					+ Region.SrcY * SrcPitch
					+ Region.SrcX * SrcBpp
				);

				DataCleanupFunc(SrcData);
			}
		}
		);
}

bool FSimpleRenderer::LoadImageToTexture2D(const FString& ImagePath, UTexture2D*& InTexture, float& Width, float& Height)
{
	TArray<uint8> ImageReasultData;
	FFileHelper::LoadFileToArray(ImageReasultData, *ImagePath);
	FString Ex = FPaths::GetExtension(ImagePath, false);

	EImageFormat ImageFormat = EImageFormat::Invalid;
	if (Ex.Equals(TEXT("jpg"), ESearchCase::IgnoreCase) || Ex.Equals(TEXT("jpeg"), ESearchCase::IgnoreCase))
	{
		ImageFormat = EImageFormat::JPEG;
	}
	else if (Ex.Equals(TEXT("bmp"), ESearchCase::IgnoreCase))
	{
		ImageFormat = EImageFormat::BMP;
	}
	else if (Ex.Equals(TEXT("png"), ESearchCase::IgnoreCase))
	{
		ImageFormat = EImageFormat::PNG;
	}

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");
	TSharedPtr<IImageWrapper> ImageWrapperptr = ImageWrapperModule.CreateImageWrapper(ImageFormat);

	if (ImageWrapperptr.IsValid() && ImageWrapperptr->SetCompressed(ImageReasultData.GetData(), ImageReasultData.GetAllocatedSize()))
	{
		TArray<uint8> OutRawData;
		ImageWrapperptr->GetRaw(ERGBFormat::BGRA, 8, OutRawData);
		Width = (float)ImageWrapperptr->GetWidth();
		Height = (float)ImageWrapperptr->GetHeight();

		InTexture = UTexture2D::CreateTransient((int32)Width, (int32)Height, PF_B8G8R8A8);
		if (InTexture)
		{
			FTexturePlatformData* PlatformData = InTexture->GetPlatformData();
			void* TextureData = PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, OutRawData.GetData(), OutRawData.Num());
			PlatformData->Mips[0].BulkData.Unlock();
			InTexture->UpdateResource();
			return true;
		}
	}
	return false;
}

bool FSimpleRenderer::LoadImageToTexture2DEx(const FString& ImagePath, TArray<uint8>& OutRawData, float& Width, float& Height)
{
	TArray<uint8> ImageReasultData;
	FFileHelper::LoadFileToArray(ImageReasultData, *ImagePath);
	FString Ex = FPaths::GetExtension(ImagePath, false);

	EImageFormat ImageFormat = EImageFormat::Invalid;
	if (Ex.Equals(TEXT("jpg"), ESearchCase::IgnoreCase) || Ex.Equals(TEXT("jpeg"), ESearchCase::IgnoreCase))
	{
		ImageFormat = EImageFormat::JPEG;
	}
	else if (Ex.Equals(TEXT("bmp"), ESearchCase::IgnoreCase))
	{
		ImageFormat = EImageFormat::BMP;
	}
	else if (Ex.Equals(TEXT("png"), ESearchCase::IgnoreCase))
	{
		ImageFormat = EImageFormat::PNG;
	}

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");
	TSharedPtr<IImageWrapper> ImageWrapperptr = ImageWrapperModule.CreateImageWrapper(ImageFormat);

	if (ImageWrapperptr.IsValid() && ImageWrapperptr->SetCompressed(ImageReasultData.GetData(), ImageReasultData.GetAllocatedSize()))
	{
		ImageWrapperptr->GetRaw(ERGBFormat::BGRA, 8, OutRawData);
		Width = (float)ImageWrapperptr->GetWidth();
		Height = (float)ImageWrapperptr->GetHeight();
		return true;
	}
	return false;
}

bool FSimpleRenderer::SaveRenderTargetToFile(UTextureRenderTarget2D* rt, const FString& fileDestination)
{
	FTextureRenderTargetResource* rtResource = rt->GameThread_GetRenderTargetResource();
	FReadSurfaceDataFlags readPixelFlags(RCM_UNorm);

	TArray<FColor> outBMP;
	outBMP.AddUninitialized(rt->GetSurfaceWidth() * rt->GetSurfaceHeight());
	rtResource->ReadPixels(outBMP, readPixelFlags);

	FIntPoint destSize(rt->GetSurfaceWidth(), rt->GetSurfaceHeight());
	TArray<uint8, FDefaultAllocator64> CompressedBitmap;
	FImageUtils::PNGCompressImageArray(destSize.X, destSize.Y, outBMP, CompressedBitmap);
	return FFileHelper::SaveArrayToFile(CompressedBitmap, *fileDestination);
}
