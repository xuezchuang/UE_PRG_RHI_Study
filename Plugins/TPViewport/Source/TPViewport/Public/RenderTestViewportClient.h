#pragma once

#include "CoreMinimal.h"
#include "UnrealClient.h"
#include "../SimpleShader/SimpleShader.h"
#include "EditorViewportClient.h"
//#include "SimpleShader.h"
//#include "SimpleShader/SimpleShader.h"

class FRenderTestViewportClient : public FEditorViewportClient
//class FRenderTestViewportClient : public FViewportClient
{
public:
	FRenderTestViewportClient();

	//virtual void Draw(FViewport* Viewport, FCanvas* Canvas)override;

	virtual void DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& InCanvas)override;

	class UTextureRenderTarget2D* RenderTarget;

	class UTexture2D* InTexture;

	float Width;
	float Height;
	TArray<uint8>OutRawData;
	FSimpleRenderer renderer;
};