#include "RenderTestViewportClient.h"
#include "Runtime/Engine/Public/CanvasTypes.h"
#include "Runtime/Engine/Public/CanvasItem.h"
#include "Engine/TextureRenderTarget2D.h"
#include "SEditorViewport.h"
#include "Templates/SharedPointer.h"


FRenderTestViewportClient::FRenderTestViewportClient()
	: FEditorViewportClient(nullptr)
{
	FString FileName = FPaths::Combine(FPaths::GameSourceDir(), TEXT("data\\Result.PNG"));
	renderer.LoadImageToTexture2D(FileName, InTexture, Width, Height);
	RenderTarget = NewObject<UTextureRenderTarget2D>();
	RenderTarget->RenderTargetFormat = RTF_RGBA8;
	RenderTarget->InitAutoFormat(1024, 1024);
	RenderTarget->ClearColor = FLinearColor::White;
	RenderTarget->bAutoGenerateMips = false;
}

static TAutoConsoleVariable CVarMyTest(
	TEXT("r.MyTest"),
	1,
	TEXT("Test My Global Shader, set it to 0 to disable, or to 1, 2 or 3 for fun!"),
	ECVF_RenderThreadSafe
);

//void FRenderTestViewportClient::Draw(FViewport* Viewport, FCanvas* Canvas)
void FRenderTestViewportClient::DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& InCanvas)
{
	Viewport = &InViewport;
	FCanvas* Canvas = &InCanvas;
	int32 MyTestValue = CVarMyTest.GetValueOnAnyThread();
	if (MyTestValue == 0)
	{
		// 定义三个顶点的位置
		FVector2D V0 = FVector2D(100, 100);  // 顶点1
		FVector2D V1 = FVector2D(Viewport->GetSizeXY().X - 100, 100);  // 顶点2
		FVector2D V2 = FVector2D(Viewport->GetSizeXY().X / 2, Viewport->GetSizeXY().Y - 100);  // 顶点3

		// 定义每个顶点的颜色
		FLinearColor Color0 = FLinearColor::Red;
		FLinearColor Color1 = FLinearColor::Green;
		FLinearColor Color2 = FLinearColor::Blue;

		// 创建三角形项目
		UTexture* WhiteTexture = LoadObject<UTexture>(NULL, TEXT("/Engine/EngineMaterials/DefaultWhiteGrid.DefaultWhiteGrid"), NULL, LOAD_None, NULL);

		FCanvasTriangleItem Triangle(V0, V1, V2, WhiteTexture->GetResource());
		Triangle.SetColor(FLinearColor::White); // 基础颜色，可以是白色，以显示顶点的真实颜色

		// 设置顶点颜色
		Triangle.TriangleList[0].V0_Color = Color0;
		Triangle.TriangleList[0].V1_Color = Color1;
		Triangle.TriangleList[0].V2_Color = Color2;

		// 绘制三角形
		Canvas->DrawItem(Triangle);

	}
	else if (MyTestValue == 1)
	{
		renderer.Render(RenderTarget,[](){});
		FCanvasTileItem Tile(FVector2D::ZeroVector, RenderTarget->GetResource(), Viewport->GetSizeXY(), FLinearColor::White);

		Canvas->DrawItem(Tile);
	}
	else if (MyTestValue == 2)
	{
		FCanvasTileItem Tile(FVector2D::ZeroVector, InTexture->GetResource(), Viewport->GetSizeXY(), FLinearColor::Black);
		Canvas->DrawItem(Tile);
	}

	FString FileName= FPaths::Combine(FPaths::GameSourceDir(),TEXT("data\\Result.PNG"));
	renderer.SaveRenderTargetToFile(RenderTarget, FileName);
}