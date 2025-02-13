
#include "SimpleShader.h"

#include "Containers/DynamicRHIResourceArray.h"
#include "TextureResource.h"
#include "Engine/TextureRenderTarget2D.h"

#include "RHIStaticStates.h"
#include "PipelineStateCache.h"

#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "ImageUtils.h"

#include "RenderResource.h"
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
		//FRHIResourceCreateInfo CreateInfo(TEXT("FSimpleVertexBuffer"));
		//VertexBufferRHI = RHICreateBuffer(sizeof(FColorVertex) * 3, BUF_Static | BUF_VertexBuffer, 0, ERHIAccess::VertexOrIndexBuffer, CreateInfo);
		//FColorVertex* Vertices = (FColorVertex*)RHILockBuffer(VertexBufferRHI, 0, sizeof(FColorVertex) * 3, RLM_WriteOnly);
		//Vertices[0].Position = FVector4f(0.0f, 0.0f, 0.5f, 1.0f);
		//Vertices[0].Color = FVector4f(1.0f, 0.0f, 0.0f, 1.0f);
		//Vertices[1].Position = FVector4f(1.0f, 0.0f, 0.5f, 1.0f);
		//Vertices[1].Color = FVector4f(0.0f, 1.0f, 0.0f, 1.0f);
		//Vertices[2].Position = FVector4f(0.0f, 1.0f, 0.5f, 1.0f);
		//Vertices[2].Color = FVector4f(0.0f, 0.0f, 1.0f, 1.0f);
		//RHIUnlockBuffer(VertexBufferRHI);

		TResourceArray<FColorVertex, VERTEXBUFFER_ALIGNMENT> Vertices;
		Vertices.SetNumUninitialized(3);
		
		Vertices[0].Position = FVector4f(-0.5f, -0.5f, 0.0f, 1.0f);
		Vertices[0].Color = FVector4f(1.0f, 0.0f, 0.0f, 1.0f);
		Vertices[1].Position = FVector4f(0.5f, -0.5f, 0.0f, 1.0f);
		Vertices[1].Color = FVector4f(0.0f, 1.0f, 0.0f, 1.0f);
		Vertices[2].Position = FVector4f(0.0f, 0.5f, 0.0f, 1.0f);
		Vertices[2].Color = FVector4f(0.0f, 0.0f, 1.0f, 1.0f);

		//FRHIResourceCreateInfo CreateInfo(&Vertices);
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
		const uint16 Indices[] = { 0, 1, 2/*, 1, 2, 3*/ };

		TResourceArray <uint16, INDEXBUFFER_ALIGNMENT> IndexBuffer;
		uint32 NumIndices = UE_ARRAY_COUNT(Indices);
		IndexBuffer. AddUninitialized (NumIndices);
		FMemory::Memcpy(IndexBuffer.GetData(), Indices, NumIndices * sizeof(uint16));

		FRHIResourceCreateInfo CreateInfo(TEXT("FSimpleIndexBuffer"));
		CreateInfo.ResourceArray = &IndexBuffer;

		IndexBufferRHI = RHICreateIndexBuffer(sizeof(uint16), IndexBuffer.GetResourceDataSize(), BUF_Static, CreateInfo);
	}
};

TGlobalResource<FSimpleIndexBuffer> GSimpleIndexBuffer;

TResourceArray<FColorVertex, VERTEXBUFFER_ALIGNMENT> mTestVertices;

static void DrawTestShaderRenderTarget_RenderThread(
	FRHICommandListImmediate& RHICmdList,
	FTextureRenderTargetResource* OutputRenderTargetResource,
	//const FVector2D& PixelUVSizeValue
	int32 nType)
{
	check(IsInRenderingThread());
	FRHITexture2D* RenderTargetTexture = OutputRenderTargetResource->GetRenderTargetTexture();
	//RHICmdList.Transition(FRHITransitionInfo(RenderTargetTexture, ERHIAccess::SRVMask, ERHIAccess::RTV));
	//RHICmdList.TransitionResource(EResourceTransitionAccess::EWritable, RenderTargetTexture);

	//FRHIRenderPassInfo RPInfo(OutputRenderTargetResource->GetRenderTargetTexture(), ERenderTargetActions::DontLoad_Store, OutputRenderTargetResource->TextureRHI);
	FRHIRenderPassInfo RPInfo(RenderTargetTexture, ERenderTargetActions::DontLoad_Store);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("SimpleRendererShaderPass"));
	{
		FIntPoint DisplacementMapResolution(OutputRenderTargetResource->GetSizeX(), OutputRenderTargetResource->GetSizeY());

		// Update viewport.
		RHICmdList.SetViewport(
			0, 0, 0.f,
			DisplacementMapResolution.X, DisplacementMapResolution.Y, 1.f);

		// Get shaders.
		FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
		TShaderMapRef<FSimpleShaderVS> VertexShader(GlobalShaderMap);
		TShaderMapRef<FSimpleShaderPS> PixelShader(GlobalShaderMap);

		// Set the graphic pipeline state.
		FGraphicsPipelineStateInitializer GraphicsPSOInit;
		RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
		GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
		GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
		GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
		GraphicsPSOInit.PrimitiveType = PT_TriangleList;// PT_TriangleList;
		GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GSimpleVertexDeclaration.VertexDeclarationRHI;
		GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
		GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
		SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);

		VertexShader->SetParameters(RHICmdList, VertexShader.GetVertexShader(), nType);
		PixelShader->SetParameters(RHICmdList, PixelShader.GetPixelShader(), nType);

		RHICmdList.SetStreamSource(0, GSimpleVertexBuffer.VertexBufferRHI, 0);

		// Update viewport.
		RHICmdList.SetViewport(
			0, 0, 0.f,
			OutputRenderTargetResource->GetSizeX(), OutputRenderTargetResource->GetSizeY(), 1.f);

		//RHICmdList.DrawIndexedPrimitive(
		//	GSimpleIndexBuffer.IndexBufferRHI,
		//	/* BaseVertexIndex= */ 0,
		//	/* MinIndex= */ 0,
		//	/* NumVertices= */ 4,  // 现在有四个顶点
		//	/* StartIndex= */ 0,
		//	/* NumPrimitives= */ 2,  // 两个三角形
		//	/* NumInstances= */ 1);

		//RHICmdList.DrawPrimitive(
		//	/* BaseVertexIndex= */ 0,
		//	/* NumPrimitives= */ 1,
		//	/* NumInstances= */ 1);


		RHICmdList.DrawIndexedPrimitive(
			GSimpleIndexBuffer.IndexBufferRHI,
			/*BaseVertexIndex=*/ 0,
			/* MinIndex = */  0,
			/* NumVertices = */3,
			/*StartIndex=*/0,
			/* NumPrimitives = */1,
			/*NumInstances=*/1);
	}
	RHICmdList.EndRenderPass();
	//RHICmdList.Transition(FRHITransitionInfo(RenderTargetTexture, ERHIAccess::RTV, ERHIAccess::SRVMask));
}


void FSimpleRenderer::Render(UTextureRenderTarget2D* RenderTarget, TFunction<void()> OnRenderCompleted)
{
	check ( IsInGameThread ());

	if (!RenderTarget)
	{
		return;
	}

	FTextureRenderTargetResource * TextureRenderTargetResource = RenderTarget-> GameThread_GetRenderTargetResource ();
	//ERHIFeatureLevel::Type FeatureLevel = World->Scene->GetFeatureLevel();
	ENQUEUE_RENDER_COMMAND(CaptureCommand)(
		[TextureRenderTargetResource, OnRenderCompleted](FRHICommandListImmediate& RHICmdList)
		{
			DrawTestShaderRenderTarget_RenderThread(RHICmdList, TextureRenderTargetResource,2);// FVector2D(0, 0));

			if (OnRenderCompleted)
				OnRenderCompleted();
		}
	);
}

void FSimpleRenderer::UpdateTextureRegion(UTextureRenderTarget2D* RenderTarget, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D Region, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, TFunction<void(uint8* SrcData)> DataCleanupFunc)
{
	check(IsInGameThread());
	FTextureRenderTargetResource* TextureRenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	ENQUEUE_RENDER_COMMAND(UpdateTextureRegionsData)(
		[=](FRHICommandListImmediate& RHICmdList)
	{
		FRHITexture2D* TextureRHI = TextureRenderTargetResource->GetTexture2DRHI();
		//check(TextureRHI->IsValid());
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
	});
}

bool FSimpleRenderer::LoadImageToTexture2D(const FString& ImagePath, UTexture2D*& InTexture, float& Width, float& Height)
{
	TArray<uint8> ImageReasultData;
	FFileHelper::LoadFileToArray(ImageReasultData, *ImagePath);//读取图片的二进制数据
	FString Ex = FPaths::GetExtension(ImagePath, false);//获取文件后缀
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
	TSharedPtr<IImageWrapper> ImageWrapperptr = ImageWrapperModule.CreateImageWrapper(ImageFormat);//创建对应格式图片壳子
	bool PtrIsValid = ImageWrapperptr.IsValid();
	if (PtrIsValid && ImageWrapperptr->SetCompressed(ImageReasultData.GetData(), ImageReasultData.GetAllocatedSize()))
	{
		TArray<uint8> OutRawData;//跟数据无关的颜色数据
		ImageWrapperptr->GetRaw(ERGBFormat::BGRA, 8, OutRawData); //按规则提取数据
		Width = ImageWrapperptr->GetWidth();
		Height = ImageWrapperptr->GetHeight();
		InTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
		if (InTexture)
		{
			FTexturePlatformData* PlatformData = InTexture->GetPlatformData(); // Use GetPlatformData() accessor
			void* TextureData = PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, OutRawData.GetData(), OutRawData.Num());//将数据写入*TextureData
			PlatformData->Mips[0].BulkData.Unlock();
			InTexture->UpdateResource();
			return true;
		}
	}
	return false;
};
bool FSimpleRenderer::LoadImageToTexture2DEx(const FString& ImagePath, TArray<uint8>& OutRawData, float& Width, float& Height)
{
	TArray<uint8> ImageReasultData;
	FFileHelper::LoadFileToArray(ImageReasultData, *ImagePath);//读取图片的二进制数据
	FString Ex = FPaths::GetExtension(ImagePath, false);//获取文件后缀
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
	TSharedPtr<IImageWrapper> ImageWrapperptr = ImageWrapperModule.CreateImageWrapper(ImageFormat);//创建对应格式图片壳子
	bool PtrIsValid = ImageWrapperptr.IsValid();
	if (PtrIsValid && ImageWrapperptr->SetCompressed(ImageReasultData.GetData(), ImageReasultData.GetAllocatedSize()))
	{
		ImageWrapperptr->GetRaw(ERGBFormat::BGRA, 8, OutRawData); //按规则提取数据
		
		/*for (int32 i = 0; i < OutRawData.Num(); i += 4) {
			UE_LOG(LogTemp, Warning, TEXT("B:%d,G:%d,R:%d,A:%d; "),OutRawData[i], OutRawData[i+1], OutRawData[i+2], OutRawData[i+3]);
		}*/
		Width = ImageWrapperptr->GetWidth();
		Height = ImageWrapperptr->GetHeight();
		return true;
	
	}
	return false;
};

bool FSimpleRenderer::SaveRenderTargetToFile(UTextureRenderTarget2D* rt, const FString& fileDestination)
{
	FTextureRenderTargetResource* rtResource = rt->GameThread_GetRenderTargetResource();
	FReadSurfaceDataFlags readPixelFlags(RCM_UNorm);

	TArray<FColor> outBMP;
	outBMP.AddUninitialized(rt->GetSurfaceWidth() * rt->GetSurfaceHeight());
	rtResource->ReadPixels(outBMP, readPixelFlags);

	FIntPoint destSize(rt->GetSurfaceWidth(), rt->GetSurfaceHeight());
	TArray<uint8, FDefaultAllocator64> CompressedBitmap;
	//FImageUtils::CompressImageArray(destSize.X, destSize.Y, outBMP, CompressedBitmap);
	// Use PNGCompressImageArray to compress the image data into PNG format
	//bool imageSavedOk = FFileHelper::SaveArrayToFile(CompressedBitmap, *fileDestination);
	FImageUtils::PNGCompressImageArray(destSize.X, destSize.Y, outBMP, CompressedBitmap);
	bool imageSavedOk = FFileHelper::SaveArrayToFile(CompressedBitmap, *fileDestination);

	return imageSavedOk;
}