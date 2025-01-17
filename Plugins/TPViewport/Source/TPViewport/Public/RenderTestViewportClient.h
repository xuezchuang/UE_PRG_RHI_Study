#pragma once

#include "CoreMinimal.h"
#include "UnrealClient.h"
#include "mTest/SimpleShader/SimpleShader.h"
//#include "SimpleShader/SimpleShader.h"

class FRenderTestViewportClient : public FViewportClient
{
public:
	FRenderTestViewportClient();

	virtual void Draw(FViewport* Viewport, FCanvas* Canvas)override;

	class UTextureRenderTarget2D* RenderTarget;

	class UTexture2D* InTexture;

	float Width;
	float Height;
	TArray<uint8>OutRawData;
	FSimpleRenderer renderer;
};