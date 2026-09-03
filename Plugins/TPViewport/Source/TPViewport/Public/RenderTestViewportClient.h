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

	// 研究阶段：完全接管 Draw，跳过 FEditorViewportClient::Draw
	// （不构建 FSceneViewFamily，不进 FDeferredShadingSceneRenderer），只走 Canvas/SimpleShader 直绘。
	virtual void Draw(FViewport* InViewport, FCanvas* InCanvas) override;
	using FEditorViewportClient::Draw; // 保留另一个 Draw(const FSceneView*, FPrimitiveDrawInterface*) 重载可见
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		FEditorViewportClient::AddReferencedObjects(Collector);
		Collector.AddReferencedObject(RenderTarget);
		Collector.AddReferencedObject(InTexture);
	}

	class UTextureRenderTarget2D* RenderTarget = nullptr;

	class UTexture2D* InTexture = nullptr;

	float Width;
	float Height;
	TArray<uint8>OutRawData;
	FSimpleRenderer renderer;
};
