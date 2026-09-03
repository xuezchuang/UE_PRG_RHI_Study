#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"

class FSimpleShader : public FGlobalShader
{
	DECLARE_INLINE_TYPE_LAYOUT(FSimpleShader, NonVirtual);
public:
	FSimpleShader() {}

	FSimpleShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		m_nType.Bind(Initializer.ParameterMap, TEXT("m_nType"));
	}

	template<typename TShaderRHIParamRef>
	void SetParameters(
		FRHICommandListImmediate& RHICmdList,
		const TShaderRHIParamRef ShaderRHI,
		const int32& nType
	)
	{
		SetShaderValue(RHICmdList, ShaderRHI, m_nType, nType);
	}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

private:
	LAYOUT_FIELD(FShaderParameter, m_nType);
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
		testColor.Bind(Initializer.ParameterMap, TEXT("testColor"));
		InputTexture.Bind(Initializer.ParameterMap, TEXT("InputTexture"), SPF_Mandatory);
		InputTextureSampler.Bind(Initializer.ParameterMap, TEXT("InputTextureSampler"), SPF_Mandatory);
	}

	void SetParameters(
		FRHICommandListImmediate& RHICmdList,
		FRHIPixelShader* ShaderRHI,
		const int32& nType,
		FRHITexture* InTextureRHI
	)
	{
		FSimpleShader::SetParameters(RHICmdList, ShaderRHI, nType);
		SetShaderValue(RHICmdList, ShaderRHI, testColor, FVector4f(1.0f, 1.0f, 0.0f, 1.0f));

		// HLSL Texture2D is exposed to D3D12 as an SRV descriptor.
		SetTextureParameter(
			RHICmdList,
			ShaderRHI,
			InputTexture,
			InputTextureSampler,
			TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(),
			InTextureRHI
		);
	}

private:
	LAYOUT_FIELD(FShaderParameter, testColor);
	LAYOUT_FIELD(FShaderResourceParameter, InputTexture);
	LAYOUT_FIELD(FShaderResourceParameter, InputTextureSampler);
};

class FSimpleRenderer
{
public:
	FSimpleRenderer() {}

	void Render(class UTextureRenderTarget2D* RenderTarget, TFunction<void()> OnRenderCompleted);
	void Render(class UTextureRenderTarget2D* RenderTarget, class UTexture2D* InputTexture, TFunction<void()> OnRenderCompleted);

	void UpdateTextureRegion(
		UTextureRenderTarget2D* RenderTarget,
		int32 MipIndex,
		uint32 NumRegions,
		FUpdateTextureRegion2D Region,
		uint32 SrcPitch,
		uint32 SrcBpp,
		uint8* SrcData,
		TFunction<void(uint8* SrcData)> DataCleanupFunc = [](uint8*) {}
	);

	bool LoadImageToTexture2D(const FString& ImagePath, UTexture2D*& InTexture, float& Width, float& Height);
	bool LoadImageToTexture2DEx(const FString& ImagePath, TArray<uint8>& OutRawData, float& Width, float& Height);
	bool SaveRenderTargetToFile(UTextureRenderTarget2D* rt, const FString& fileDestination);

};
