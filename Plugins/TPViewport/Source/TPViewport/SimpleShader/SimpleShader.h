# pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"

class FSimpleShader : public FGlobalShader
{
	DECLARE_INLINE_TYPE_LAYOUT(FSimpleShader, NonVirtual);
public:
	FSimpleShader()
	{
		int a = 0;
	}

	FSimpleShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{		
		m_nType.Bind(Initializer.ParameterMap, TEXT("m_nType"));
		testColor.Bind(Initializer.ParameterMap, TEXT("testColor"));
	}

	template<typename TShaderRHIParamRef>
	void SetParameters(
		FRHICommandListImmediate& RHICmdList,
		const TShaderRHIParamRef ShaderRHI,
		const int32& nType)
	{
		SetShaderValue(RHICmdList, ShaderRHI, m_nType, nType);
		SetShaderValue(RHICmdList, ShaderRHI, testColor,  FVector4f(1.0f));
	}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
private:
	LAYOUT_FIELD(FShaderParameter, m_nType);
	LAYOUT_FIELD(FShaderParameter, testColor);
};

class FSimpleShaderVS : public FSimpleShader
{
	DECLARE_SHADER_TYPE(FSimpleShaderVS, Global);
public:
	FSimpleShaderVS() {}

	FSimpleShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FSimpleShader(Initializer)
	{
	}
};

class FSimpleShaderPS : public FSimpleShader
{
	DECLARE_SHADER_TYPE(FSimpleShaderPS, Global);
public:
	FSimpleShaderPS() {}

	FSimpleShaderPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FSimpleShader(Initializer)
	{
	}
};

class FSimpleRenderer
{
public:
	FSimpleRenderer () {};

    /**
     * Renders the specified render target.
     *
     * @param RenderTarget The render target to be rendered.
     * @param OnRenderCompleted The callback function to be called when the rendering is completed.
     */
	void  Render(class  UTextureRenderTarget2D* RenderTarget, TFunction<void()> OnRenderCompleted);

	
	void UpdateTextureRegion(UTextureRenderTarget2D* RenderTarget, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D Region, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, TFunction<void(uint8* SrcData)> DataCleanupFunc = [](uint8*) {});
	
	bool LoadImageToTexture2D(const FString& ImagePath, UTexture2D*& InTexture, float& Width, float& Height);

	bool LoadImageToTexture2DEx(const FString& ImagePath, TArray<uint8>& OutRawData, float& Width, float& Height);

	bool SaveRenderTargetToFile(UTextureRenderTarget2D* rt, const FString& fileDestination);
};
