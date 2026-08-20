# RPG 代码研读书签

> Solution: `D:\ue\UE_PRG_RHI_Study\RPG.sln`  
> Source: Visual Studio `.suo` / `BookmarkState`  
> Exported: 2026-08-06  
> Format: Markdown file links with `#L<line>` fragments

这是从 Visual Studio 书签窗口导出的第一版代码研读索引。链接中的文件路径和行号保留了导出时的状态；笔记内容可以直接写在对应条目下面。

## CreatePSO

- [FD3D12PipelineStateCache::Init](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/Windows/WindowsD3D12PipelineState.cpp#L508) — `WindowsD3D12PipelineState.cpp:508`
- [InternalCreateGraphicsPipelineState](file:///D:/UnrealEngine/Engine/Source/Runtime/RHI/Private/PipelineStateCache.cpp#L2483) — `PipelineStateCache.cpp:2483`
- [CompilePSO](file:///D:/UnrealEngine/Engine/Source/Runtime/RHI/Private/PipelineStateCache.cpp#L1762) — `PipelineStateCache.cpp:1762`
- [RHICreateGraphicsPipelineState](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12State.cpp#L595) — `D3D12State.cpp:595`
- [CreateAndAddToLowLevelCache](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12Pipelinestate.cpp#L525) — `D3D12Pipelinestate.cpp:525`
- [CreateGraphicsPipelineState](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/Windows/WindowsD3D12PipelineState.cpp#L967) — `WindowsD3D12PipelineState.cpp:967`
- [CreatePipelineStateFromStream](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/Windows/WindowsD3D12PipelineState.cpp#L758) — `WindowsD3D12PipelineState.cpp:758`
- [GetAndOrCreateGraphicsPipelineState](file:///D:/UnrealEngine/Engine/Source/Runtime/RHI/Private/PipelineStateCache.cpp#L2578) — `PipelineStateCache.cpp:2578`
- [GetLowLevelGraphicsPipelineStateDesc](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12Pipelinestate.cpp#L51) — `D3D12Pipelinestate.cpp:51`

## Draw

- [FInstanceCullingContext::SubmitDrawCommands](file:///D:/UnrealEngine/Engine/Source/Runtime/Renderer/Private/InstanceCulling/InstanceCullingContext.cpp#L1385) — `InstanceCullingContext.cpp:1385`
- [SubmitDrawBegin](file:///D:/UnrealEngine/Engine/Source/Runtime/Renderer/Private/MeshPassProcessor.cpp#L1383) — `MeshPassProcessor.cpp:1383`
- [FMeshDrawCommand::SubmitDraw](file:///D:/UnrealEngine/Engine/Source/Runtime/Renderer/Private/MeshPassProcessor.cpp#L1577) — `MeshPassProcessor.cpp:1577`

## RHIPipelineState

- [SetGraphicsPipelineState](file:///D:/UnrealEngine/Engine/Source/Runtime/RHI/Public/RHICommandList.h#L3693) — `RHICommandList.h:3693`
- [SetGraphicsPipelineState](file:///D:/UnrealEngine/Engine/Source/Runtime/RHI/Private/PipelineStateCache.cpp#L684) — `PipelineStateCache.cpp:684`
- [FRHICommandSetGraphicsPipelineState](file:///D:/UnrealEngine/Engine/Source/Runtime/RHI/Public/RHICommandList.h#L1543) — `RHICommandList.h:1543`
- [FRHICommandSetGraphicsPipelineState::Execute](file:///D:/UnrealEngine/Engine/Source/Runtime/RHI/Public/RHICommandListCommandExecutes.inl#L275) — `RHICommandListCommandExecutes.inl:275`
- [FD3D12CommandContext::RHISetGraphicsPipelineState](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12Commands.cpp#L822) — `D3D12Commands.cpp:822`
- [SetGraphicsPipelineState](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12StateCache.cpp#L1301) — `D3D12StateCache.cpp:1301`
- [SetShaderParameter](file:///D:/UnrealEngine/Engine/Source/Runtime/RHI/Public/RHICommandList.h#L546) — `RHICommandList.h:546`
- [BindUniformBuffer](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12Commands.cpp#L92) — `D3D12Commands.cpp:92`
- [FD3D12CommandContext::RHISetShaderUniformBuffer](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12Commands.cpp#L980) — `D3D12Commands.cpp:980`

## RHIDraw -- RHIThread

- [ALLOC_COMMAND](file:///D:/UnrealEngine/Engine/Source/Runtime/RHI/Public/RHICommandList.h#L463) — `RHICommandList.h:463`
- [AllocCommand](file:///D:/UnrealEngine/Engine/Source/Runtime/RHI/Public/RHICommandList.h#L684) — `RHICommandList.h:684`
- [FRHICOMMAND_MACRO(FRHICommandDrawIndexedPrimitive)](file:///D:/UnrealEngine/Engine/Source/Runtime/RHI/Public/RHICommandList.h#L1396) — `RHICommandList.h:1396`
- [FRHICommand](file:///D:/UnrealEngine/Engine/Source/Runtime/RHI/Public/RHICommandList.h#L1105) — `RHICommandList.h:1105`
- [FRHICommandDrawIndexedPrimitive::Execute](file:///D:/UnrealEngine/Engine/Source/Runtime/RHI/Public/RHICommandListCommandExecutes.inl#L206) — `RHICommandListCommandExecutes.inl:206`
- [FD3D12CommandContext::RHIDrawIndexedPrimitive](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12Commands.cpp#L1654) — `D3D12Commands.cpp:1654`
- [EnqueueRHIThread](file:///D:/UnrealEngine/Engine/Source/Runtime/RHI/Private/RHICommandList.cpp#L880) — `RHICommandList.cpp:880`
- [SetShaderParametersOnContext](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12Commands.cpp#L1044) — `D3D12Commands.cpp:1044`
- [StateCache.SetConstantBuffer](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12Commands.cpp#L1457) — `D3D12Commands.cpp:1457`
- [FD3D12StateCache::SetConstantBuffer](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12StateCachePrivate.h#L557) — `D3D12StateCachePrivate.h:557`
- [FD3D12StateCache::ApplyState](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12StateCache.cpp#L423) — `D3D12StateCache.cpp:423`
- [FD3D12StateCache::ApplyConstants](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12StateCache.cpp#L760) — `D3D12StateCache.cpp:760`
- [FD3D12DescriptorCache::SetRootConstantBuffers](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12DescriptorCache.cpp#L594) — `D3D12DescriptorCache.cpp:594`
- [FD3D12ConstantBuffer::Version](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12ConstantBuffer.cpp#L54) — `D3D12ConstantBuffer.cpp:54`

## ENQUEUE_RENDER_COMMAND

- [ENQUEUE_RENDER_COMMAND](file:///D:/UnrealEngine/Engine/Source/Runtime/RenderCore/Public/RenderingThread.h#L294) — `RenderingThread.h:294`
- [EnqueueUniqueRenderCommand](file:///D:/UnrealEngine/Engine/Source/Runtime/RenderCore/Public/RenderingThread.h#L277) — `RenderingThread.h:277`
- [FRenderCommand](file:///D:/UnrealEngine/Engine/Source/Runtime/RenderCore/Public/RenderingThread.h#L153) — `RenderingThread.h:153`

## Task

- [TGraphTask](file:///D:/UnrealEngine/Engine/Source/Runtime/Core/Public/Async/TaskGraphInterfaces.h#L1262) — `TaskGraphInterfaces.h:1262`
- [ConstructAndDispatchWhenReady](file:///D:/UnrealEngine/Engine/Source/Runtime/Core/Public/Async/TaskGraphInterfaces.h#L1213) — `TaskGraphInterfaces.h:1213`
- [Setup](file:///D:/UnrealEngine/Engine/Source/Runtime/Core/Public/Async/TaskGraphInterfaces.h#L1405) — `TaskGraphInterfaces.h:1405`
- [SetupPrereqs](file:///D:/UnrealEngine/Engine/Source/Runtime/Core/Public/Async/TaskGraphInterfaces.h#L1386) — `TaskGraphInterfaces.h:1386`
- [PrerequisitesComplete](file:///D:/UnrealEngine/Engine/Source/Runtime/Core/Public/Async/TaskGraphInterfaces.h#L826) — `TaskGraphInterfaces.h:826`
- [QueueTask](file:///D:/UnrealEngine/Engine/Source/Runtime/Core/Public/Async/TaskGraphInterfaces.h#L932) — `TaskGraphInterfaces.h:932`
- [FTaskGraphCompatibilityImplementation::QueueTask](file:///D:/UnrealEngine/Engine/Source/Runtime/Core/Private/Async/TaskGraph.cpp#L1954) — `TaskGraph.cpp:1954`

## TaskThread

- [FNamedTaskThread::EnqueueFromOtherThread](file:///D:/UnrealEngine/Engine/Source/Runtime/Core/Private/Async/TaskGraph.cpp#L806) — `TaskGraph.cpp:806`
- [FNamedTaskThread::ProcessTasksNamedThread](file:///D:/UnrealEngine/Engine/Source/Runtime/Core/Private/Async/TaskGraph.cpp#L758) — `TaskGraph.cpp:758`
- [FStallingTaskQueue::Push](file:///D:/UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/LockFreeList.h#L682) — `LockFreeList.h:682`
- [FLockFreePointerFIFOBase::Push](file:///D:/UnrealEngine/Engine/Source/Runtime/Core/Public/Containers/LockFreeList.h#L555) — `LockFreeList.h:555`

## RenderThread

- [CreateRenderThread](file:///D:/UnrealEngine/Engine/Source/Runtime/RenderCore/Private/RenderingThread.cpp#L803) — `RenderingThread.cpp:803`
- [FRenderingThread](file:///D:/UnrealEngine/Engine/Source/Runtime/RenderCore/Private/RenderingThread.cpp#L530) — `RenderingThread.cpp:530`
- [GameThreadWaitForTask](file:///D:/UnrealEngine/Engine/Source/Runtime/RenderCore/Private/RenderingThread.cpp#L1211) — `RenderingThread.cpp:1211`
- [DrawIndexedPrimitive](file:///D:/UnrealEngine/Engine/Source/Runtime/RHI/Public/RHICommandList.h#L3573) — `RHICommandList.h:3573`
- [SetShaderParameters](file:///D:/UnrealEngine/Engine/Source/Runtime/RHI/Public/RHICommandList.h#L3302) — `RHICommandList.h:3302`

## UE_Texture

- [NewObject<UTextureRenderTarget2D>()](file:///D:/ue/UE_PRG_RHI_Study/Plugins/TPViewport/Source/TPViewport/Private/RenderTestViewportClient.cpp#L20) — `RenderTestViewportClient.cpp:20`
- [UTextureRenderTarget2D---InitAutoFormat](file:///D:/UnrealEngine/Engine/Source/Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h#L153) — `TextureRenderTarget2D.h:153`
- [UTextureRenderTarget2D::CreateResource--FTextureRenderTarget2DResource](file:///D:/UnrealEngine/Engine/Source/Runtime/Engine/Private/TextureRenderTarget2D.cpp#L65) — `TextureRenderTarget2D.cpp:65`
- [FTextureRenderTarget2DResource](file:///D:/UnrealEngine/Engine/Source/Runtime/Engine/Public/TextureResource.h#L403) — `TextureResource.h:403`

## 111

- [Bookmark37](file:///D:/UnrealEngine/Engine/Plugins/Runtime/MeshModelingToolset/Source/ModelingComponents/Private/Scene/WorldRenderCapture.cpp#L135) — `WorldRenderCapture.cpp:135`
- [Bookmark35](file:///D:/UnrealEngine/Engine/Source/Runtime/Engine/Private/UnrealClient.cpp#L85) — `UnrealClient.cpp:85`
- [Bookmark38](file:///D:/UnrealEngine/Engine/Source/Runtime/Renderer/Private/DeferredShadingRenderer.cpp#L3274) — `DeferredShadingRenderer.cpp:3274`
- [WaitTime](file:///D:/UnrealEngine/Engine/Source/Runtime/Engine/Private/UnrealEngine.cpp#L2599) — `UnrealEngine.cpp:2599`
- [UnrealClient->Draw](file:///D:/UnrealEngine/Engine/Source/Runtime/Engine/Private/UnrealClient.cpp#L1850) — `UnrealClient.cpp:1850`
- [SlateRHIRender--DrawWindow_RenderThread](file:///D:/UnrealEngine/Engine/Source/Runtime/SlateRHIRenderer/Private/SlateRHIRenderer.cpp#L1722) — `SlateRHIRenderer.cpp:1722`
- [Bookmark32](file:///D:/UnrealEngine/Engine/Source/Runtime/Core/Private/Async/TaskGraph.cpp#L2069) — `TaskGraph.cpp:2069`
- [ViewportInterfacePin--OnDrawViewport](file:///D:/UnrealEngine/Engine/Source/Runtime/Slate/Private/Widgets/SViewport.cpp#L152) — `SViewport.cpp:152`
- [BeginInitResource](file:///D:/UnrealEngine/Engine/Source/Runtime/RenderCore/Private/RenderResource.cpp#L373) — `RenderResource.cpp:373`
- [FGlobalShaderMap](file:///D:/UnrealEngine/Engine/Source/Runtime/RenderCore/Public/GlobalShader.h#L193) — `GlobalShader.h:193`
- [Bookmark40](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12Texture.cpp#L873) — `D3D12Texture.cpp:873`
- [Bookmark41](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12View.cpp#L66) — `D3D12View.cpp:66`
- [Bookmark42](file:///D:/UnrealEngine/Engine/Source/Runtime/D3D12RHI/Private/D3D12View.cpp#L80) — `D3D12View.cpp:80`
- [Bookmark43](file:///D:/UnrealEngine/Engine/Source/Runtime/RHI/Public/RHICommandList.h#L3362) — `RHICommandList.h:3362`
- [Bookmark44](file:///D:/UnrealEngine/Engine/Source/Runtime/Slate/Private/Widgets/Input/SCheckBox.cpp#L363) — `SCheckBox.cpp:363`
- [Bookmark45](file:///D:/UnrealEngine/Engine/Source/Runtime/SlateCore/Private/Styling/StarshipCoreStyle.cpp#L1532) — `StarshipCoreStyle.cpp:1532`
