# AGENTS.md

Local instructions for `D:\ue\UE_PRG_RHI_Study`.

This repository is a systematic Unreal Engine source-analysis and RHI learning workspace. Its primary purpose is to study and explain how UE actually works across source layers; the experiment project and browser-readable learning site support that investigation.

## Scope

- UE project: `RPG.uproject`
- Source modules: `Source/`
- RHI and rendering experiments: `Plugins/`, especially `TPViewport` and `D3D12RHIAccess`
- Learning website: `docs/render-notes-site/`
- Existing markdown source notes: `D:\ue\UERenderNotes`
- Unreal Engine source checkout: `D:\UnrealEngine`
- Primary D3D12 comparison engine (user-built, UE-inspired): `D:\graph\SnowEngine`
- Minimal D3D12 sample (Frank Luna d3d12book): `D:\graph\d3d12book\Chapter 21 Ambient Occlusion\Ssao`

## UE Source-Analysis Mission

The long-term goal is to research, dissect, and understand Unreal Engine source code. Do not reduce the repository goal to one feature, one plugin, the Material Editor, or BasePass. Material preview rendering is only the current confirmed study topic and one worked end-to-end route.

Source investigations may cover, among other areas:

- engine/editor startup, object systems, modules, worlds, and asset lifecycles
- Slate, UMG, editor viewports, and UI-to-renderer boundaries
- game thread, render thread, RHI thread, task graph, and synchronization
- scene proxies, primitive data, mesh batches, mesh processors, and mesh draw commands
- materials, generated HLSL, shader types, permutations, shader maps, DDC, and shader compilation
- renderer passes, RDG resources/passes, views, scene textures, lighting, shadows, and post processing
- RHI abstractions and D3D12 command lists, descriptors, resource states, root signatures, PSOs, barriers, and GPU submission
- CPU/GPU data ownership, lifetime, caching, invalidation, and update paths

For every source-learning question, first identify the exact current target. Then build the smallest complete chain that answers it. A useful investigation normally records:

1. the user-visible trigger or the lowest-level entry requested by the user
2. the real callers and callees, with concrete source paths and symbols
3. the important input/output data structures and who owns them
4. lifetime, cache/invalidation behavior, and the executing thread where relevant
5. generated code, compile boundaries, runtime binding, and GPU submission as separate stages
6. what was verified directly in source, what is inferred, and what remains unverified

Do not force unrelated UE questions through the material-preview route below. Use that route only when it is relevant, and add a new focused route when the user explicitly asks to preserve another confirmed investigation.

## Working Rules

- Read this file before changing the project.
- Treat Unreal generated folders as build/runtime output: `Binaries/`, `Intermediate/`, `DerivedDataCache/`, `.vs/`, and `Saved/`.
- Do not make broad formatting-only changes to UE C++ or shader files.
- Preserve existing user edits in plugins and content folders unless the user explicitly asks to change them.
- For RHI/D3D12 work, trace the actual call path and data ownership before patching.
- For shader changes, include the affected `.usf` or C++ binding path in the report.
- For web-note work, keep it static and easy to open locally unless the user asks for a framework.

## Current Confirmed Study Topic

The current worked topic is the material preview sphere in the Material Editor. For this topic, the target is **not** the Slate painting of the material graph nodes. The user wants to start at the low-level shader entry, establish data and compilation semantics, and then connect that path upward to the preview viewport.

- Engine source root: `D:\UnrealEngine`
- Verified engine version: UE 5.2.1 (`D:\UnrealEngine\Engine\Build\Build.version`)
- Primary D3D12 comparison engine: `D:\graph\SnowEngine`. The user reports understanding ~90% of its D3D12 side, so it is the preferred "known side" of UE analogies. See "Comparison Engine: SnowEngine" below.
- Minimal D3D12 sample for smallest-case comparisons: `D:\graph\d3d12book\Chapter 21 Ambient Occlusion\Ssao`
- Treat line numbers below as UE 5.2.1 navigation anchors. If the engine checkout changes, search the named symbol again instead of trusting the old line number.

### General Research Behavior

- Do not begin with a broad scan of the entire engine when the target is already known. Start from confirmed symbols and follow their real callers, callees, generated includes, bindings, and data owners.
- Do not delegate or launch parallel exploratory agents for a documented route by default. Keep each call chain coherent in one investigation. Delegate only when the user explicitly asks, or when a genuinely independent unknown cannot be resolved efficiently on the main path.
- For explanation or call-chain requests, remain read-only unless the user explicitly requests implementation.
- Distinguish verified source facts from inference and from unverified runtime behavior.
- Explain source in terms of control flow, data flow, ownership, lifetime, thread, and CPU/GPU boundary instead of listing filenames without their relationships.
- When shader code is involved, do not confuse a shared HLSL entry name with a single compiled shader binary; material, vertex-factory, light-map, feature, and permutation defines can produce many binaries.
- Preferred explanation method for rendering/RHI topics is comparison-first: anchor the explanation in the SnowEngine equivalent the user already understands (render layers, root signatures, descriptor heaps, constant buffers, PSOs), then show how UE generalizes it (RHI abstraction layer, mesh pass processors, RDG, shader permutations, thread split). Explicitly name what UE has that SnowEngine deliberately omits instead of presenting the two as identical.

## Comparison Engine: SnowEngine

`D:\graph\SnowEngine` is a UE-inspired C++/DirectX 12 engine and editor the user built as a learning project (rendering pipeline, reflection/object system, Slate-style editor, Blueprint-style scripting). The user understands ~90% of its D3D12 side, so it is the primary baseline when dissecting UE by comparison; the Frank Luna Ssao sample stays the minimal-case reference.

Verified layout of the D3D12 rendering core (paths below are relative to `D:\graph\SnowEngine\Source\snowEngine\Engine\Rendering\Core\DirectX\RenderingPipeline\`):

- Pipeline orchestration: `RenderingPipeline.cpp/.h`, `RenderingPipelineType.cpp/.h`
- Root signatures: `RootSignature\` (`DirectXRootSignature.h`, `DefaultDirectXRootSignature.h`, `SSAODirectXRootSignature.h`, `DirectXRootSignatureType.h`)
- PSO: `PipelineState\DirectXPipelineState.cpp/.h`
- Descriptor heaps: `DescriptorHeap\DirectXDescriptorHeap.cpp/.h`
- Per-frame constant buffers: `ConstantBuffer\ConstantBufferViews.cpp/.h`
- Render layers (per draw category): `RenderLayer\` (`RenderLayerManage`, `OpaqueRenderLayer`, `OpaqueSkinnedRenderLayer`, `AlphaTestRenderLayer`, `TransparentRenderLayer`, `SSAORenderLayer`, `OpaqueShadowRenderLayer`, `SelectRenderLayer`, editor-handle layers)
- Render targets / buffers: `RenderBuffer\` (`DepthBuffer`, `NormalBuffer`, `AmbientBuffer`, `NoiseBuffer`, `SampleVolumeBuffer`)
- AO variants: `AmbientOcclusion\` (MiniEngine SSAO, HBAO+, screen-space)
- Geometry / vertex construction: `Geometry\`, `..\Buffer\ConstructBuffer.cpp/.h`
- HLSL: `D:\graph\SnowEngine\Shaders\` (`PBR.hlsl`, `BRDF.hlsl`, `Material.hlsl`, `Light.hlsl`, `Shadow.hlsl`, `NormalBuffer.hlsl`, `AO\`, per-editor-tool shaders)

Starting-point mappings; verify the actual call sites before asserting them in a report:

```text
SnowEngine RenderLayer / RenderLayerManage   ~ UE FMeshPassProcessor draws + FDeferredShadingSceneRenderer pass sequence
SnowEngine RootSignature (hand-built)         ~ UE FD3D12RootSignature (cached, layout/parameter-driven)
SnowEngine DirectXPipelineState               ~ UE D3D12 PSO creation + pipeline cache
SnowEngine DirectXDescriptorHeap              ~ UE D3D12 descriptor heap managers and view creation
SnowEngine ConstantBufferViews                ~ UE uniform buffers / constant-buffer update path
SnowEngine RenderBuffer (Depth/Normal/...)    ~ UE pooled render targets and FRDGTexture graph resources
SnowEngine RenderingPipeline                  ~ UE FDeferredShadingSceneRenderer::Render
SnowEngine Shaders/*.hlsl per technique       ~ UE .usf/.ush templates + generated material code + permutations
```

Key asymmetries to call out when comparing (do not paper over these):

- SnowEngine calls D3D12 directly; UE inserts the platform-agnostic RHI layer (`FRHIResource` and friends) plus the render-thread/RHI-thread split.
- SnowEngine shaders are hand-written per technique; UE generates HLSL from material graphs and multiplies binaries via vertex factory, feature level, quality, and permutation defines.
- SnowEngine owns render targets manually per technique; UE routes allocation through the render-target pool and RDG (transient aliasing, automatic barriers, pass/resource culling).

## Current Route: Material Preview and BasePass

Follow this order unless the user narrows the question to one layer.

### 1. BasePass pixel-shader entry and material output

- BasePass PS registration: `D:\UnrealEngine\Engine\Source\Runtime\Renderer\Private\BasePassRendering.cpp:126`
- Shared shader file: `D:\UnrealEngine\Engine\Shaders\Private\BasePassPixelShader.usf`
- The registered HLSL entry point is `MainPS`, not `FPixelShaderInOut_MainPS`.
- `MainPS` is defined by `PixelShaderOutputCommon.ush` and calls the BasePass body `FPixelShaderInOut_MainPS`.
- `BasePassPixelShader.usf` includes the virtual generated file `/Engine/Generated/Material.ush`.
- Continue through `CalcMaterialParameters` / `CalcMaterialParametersEx`, generated `CalcPixelMaterialInputs`, `FPixelMaterialInputs.BaseColor`, and `GetMaterialBaseColor`.

Primary anchors:

- `D:\UnrealEngine\Engine\Shaders\Private\BasePassPixelShader.usf:64`
- `D:\UnrealEngine\Engine\Shaders\Private\BasePassPixelShader.usf:757`
- `D:\UnrealEngine\Engine\Shaders\Private\BasePassPixelShader.usf:2194`
- `D:\UnrealEngine\Engine\Shaders\Private\PixelShaderOutputCommon.ush:12`
- `D:\UnrealEngine\Engine\Shaders\Private\PixelShaderOutputCommon.ush:136`
- `D:\UnrealEngine\Engine\Shaders\Private\MaterialTemplate.ush:3107`

Use this mental model:

```text
MainPS
  -> FPixelShaderInOut_MainPS
  -> CalcMaterialParameters[Ex]
  -> generated CalcPixelMaterialInputs
  -> FPixelMaterialInputs.BaseColor / Metallic / Roughness / ...
  -> BasePass lighting and MRT/GBuffer output
```

`FPixelMaterialInputs` is a stable material-output interface. It is not proof that every material uses one fixed GPU-side storage structure.

### 2. Material graph translation and shader-map compilation

- Start at `FMaterial::BeginCompileShaderMap` in `D:\UnrealEngine\Engine\Source\Runtime\Engine\Private\Materials\MaterialShared.cpp:3050`.
- Follow `Translate`, `HLSLMaterialTranslator`, `GetMaterialShaderCode`, `UniformExpressionSet.CreateBufferStruct`, and `FMaterialShaderMap::Compile`.
- The generated material code is mapped to `/Engine/Generated/Material.ush` and combined with pass shader templates and permutation defines.
- `BeginCompileShaderMap` performs real material shader-map generation/compilation. It is not the equivalent of `SsaoApp::BuildRootSignature`.
- DDC may supply existing results, so entering this function does not prove that DXC compiled every job from scratch.

Primary anchors:

- `D:\UnrealEngine\Engine\Source\Runtime\Engine\Private\Materials\MaterialShared.cpp:2992`
- `D:\UnrealEngine\Engine\Source\Runtime\Engine\Private\Materials\MaterialShared.cpp:3007`
- `D:\UnrealEngine\Engine\Source\Runtime\Engine\Private\Materials\MaterialShared.cpp:3050`
- `D:\UnrealEngine\Engine\Source\Runtime\Engine\Private\Materials\MaterialShared.cpp:3078`
- `D:\UnrealEngine\Engine\Source\Runtime\Engine\Private\Materials\MaterialShared.cpp:3102`
- `D:\UnrealEngine\Engine\Source\Runtime\Engine\Private\Materials\HLSLMaterialTranslator.cpp:2258`

### 3. BaseColor parameter data ownership

Keep these node types separate:

- `Vector Parameter`: translated to a numeric uniform expression; its default/runtime value is supplied through the material uniform/preshader path rather than normally becoming a literal pixel-shader instruction.
- `Constant3Vector`: translated as an HLSL literal and can be constant-folded into compiled shader code.
- `Static Switch Parameter`: changes compile-time code/permutations and therefore selects or produces different shaders.
- Dynamic material-instance parameter updates: update runtime uniform data and do not call `BeginCompileShaderMap` merely to change the value.

Trace the screenshot's `Vector Parameter` through:

```text
UMaterialExpressionVectorParameter::Compile
  -> FMaterialCompiler::VectorParameter
  -> HLSLMaterialTranslator::NumericParameter
  -> FMaterialUniformExpressionNumericParameter
  -> UniformExpressionSet / material preshader
  -> Material.PreshaderBuffer
  -> material uniform buffer creation or update
  -> FMaterialShader::GetShaderBindings
```

Primary anchors:

- `D:\UnrealEngine\Engine\Source\Runtime\Engine\Private\Materials\MaterialExpressions.cpp:8497`
- `D:\UnrealEngine\Engine\Source\Runtime\Engine\Private\Materials\HLSLMaterialTranslator.cpp:4464`
- `D:\UnrealEngine\Engine\Source\Runtime\Engine\Private\Materials\MaterialUniformExpressions.cpp:27`
- `D:\UnrealEngine\Engine\Source\Runtime\Engine\Private\Materials\MaterialUniformExpressions.cpp:737`
- `D:\UnrealEngine\Engine\Source\Runtime\Engine\Private\Materials\MaterialRenderProxy.cpp:357`
- `D:\UnrealEngine\Engine\Source\Runtime\Renderer\Private\ShaderBaseClasses.cpp:356`

Changing a parameter default on the base `UMaterial` can still trigger a conservative shader-map refresh because the compiled material resource also carries uniform layout, defaults, preshader metadata, and shader references. Do not describe that editor refresh as proof that the vector value itself was baked into DXIL.

### 4. Preview sphere to renderer

The confirmed upper path begins in the Material Editor:

```text
SMaterialEditor3DPreviewViewport::Construct
  -> create FAdvancedPreviewScene
  -> SetPreviewAsset(EditorSphere)
  -> create UMaterialEditorMeshComponent / UStaticMeshComponent
  -> SetPreviewMaterial
  -> PreviewMeshComponent->OverrideMaterials.Add
  -> MarkRenderStateDirty
  -> FMaterialEditorViewportClient::Draw
  -> FEditorViewportClient::Draw
  -> build FSceneViewFamily / FSceneView
  -> FRendererModule::BeginRenderingViewFamily
  -> FSceneRenderer::CreateSceneRenderer
  -> FDeferredShadingSceneRenderer::Render
```

Primary anchors:

- `D:\UnrealEngine\Engine\Source\Editor\MaterialEditor\Private\SMaterialEditorViewport.cpp:118`
- `D:\UnrealEngine\Engine\Source\Editor\MaterialEditor\Private\SMaterialEditorViewport.cpp:234`
- `D:\UnrealEngine\Engine\Source\Editor\MaterialEditor\Private\SMaterialEditorViewport.cpp:267`
- `D:\UnrealEngine\Engine\Source\Editor\MaterialEditor\Private\SMaterialEditorViewport.cpp:331`
- `D:\UnrealEngine\Engine\Source\Editor\MaterialEditor\Private\SMaterialEditorViewport.cpp:431`
- `D:\UnrealEngine\Engine\Source\Editor\UnrealEd\Private\EditorViewportClient.cpp:3940`
- `D:\UnrealEngine\Engine\Source\Editor\UnrealEd\Private\EditorViewportClient.cpp:4118`
- `D:\UnrealEngine\Engine\Source\Runtime\Renderer\Private\SceneRendering.cpp:3917`
- `D:\UnrealEngine\Engine\Source\Runtime\Renderer\Private\SceneRendering.cpp:4527`
- `D:\UnrealEngine\Engine\Source\Runtime\Renderer\Private\DeferredShadingRenderer.cpp:2411`

Next connect the renderer to BasePass mesh commands through:

- `FDeferredShadingSceneRenderer::RenderBasePass`
- `FBasePassMeshProcessor::AddMeshBatch`
- `FBasePassMeshProcessor::Process`
- `BuildMeshDrawCommands`
- `FMeshMaterialShader::GetShaderBindings`

Anchors are `BasePassRendering.cpp:951`, `BasePassRendering.cpp:1699`, `BasePassRendering.cpp:1817`, and `BasePassRendering.cpp:1834`.

### 5. D3D12 Root Signature and PSO boundary

Keep shader compilation, parameter binding, Root Signature creation, and PSO creation as separate stages. The Frank Luna mappings below are the minimal-case version; for the same boundary against the user's own engine, use the SnowEngine mappings in "Comparison Engine: SnowEngine".

- Frank Luna sample: `SsaoApp::BuildRootSignature` describes the binding ABI for `b0`, `b1`, `t0 space1`, and descriptor tables. It does not translate a material graph or compile its HLSL.
- UE analogue: material shader compilation produces bytecode and binding metadata; D3D12RHI later obtains/creates a cached Root Signature and uses it in a graphics PSO.

UE anchors:

- `D:\UnrealEngine\Engine\Source\Runtime\D3D12RHI\Private\D3D12RootSignature.cpp:419`
- `D:\UnrealEngine\Engine\Source\Runtime\D3D12RHI\Private\D3D12RootSignature.cpp:449`
- `D:\UnrealEngine\Engine\Source\Runtime\D3D12RHI\Private\D3D12RootSignature.cpp:457`
- `D:\UnrealEngine\Engine\Source\Runtime\D3D12RHI\Private\D3D12RootSignature.cpp:674`
- `D:\UnrealEngine\Engine\Source\Runtime\D3D12RHI\Private\D3D12Pipelinestate.cpp:25`

D3D12 book comparison anchors:

- `D:\graph\d3d12book\Chapter 21 Ambient Occlusion\Ssao\Shaders\Common.hlsl`: `MaterialData`, `gMaterialData`, `cbPerObject`, and `cbPass`
- `D:\graph\d3d12book\Chapter 21 Ambient Occlusion\Ssao\Shaders\Default.hlsl`: material lookup and VS/PS use
- `D:\graph\d3d12book\Chapter 21 Ambient Occlusion\Ssao\FrameResource.h`: CPU-side `MaterialData`
- `D:\graph\d3d12book\Chapter 21 Ambient Occlusion\Ssao\SsaoApp.cpp`: `BuildRootSignature`, material SRV binding, object CBV binding, and draw

When explaining the comparison, use this mapping as a starting point, then verify the actual call sites:

```text
D3D12 book MaterialData / gMaterialData
  ~ UE material uniform expressions / preshader buffer / material uniform buffer

D3D12 book Default.hlsl::PS
  ~ UE MainPS -> FPixelShaderInOut_MainPS -> generated material evaluation

D3D12 book BuildRootSignature
  ~ UE D3D12RHI Root Signature creation, downstream of material compilation
```

## Render Learning Website

The local learning site lives at:

- `docs/render-notes-site/index.html`

The main site is web-first, not a Markdown reader:

- `docs/render-notes-site/learning-data.js` contains curated modules, routes, source maps, labs, and terms
- `docs/render-notes-site/app.js` renders the single-page learning UI
- `D:\ue\UERenderNotes` is raw reference material only

The site is intentionally single-page:

- left navigation switches between learning modes
- module links jump into curated topic pages
- search covers modules, source areas, labs, and terms
- browser hashes preserve deep links

## UE Source Research Blog

`docs/ue-research.html` is a single-file static blog (no framework, no dependencies, opens directly over `file://`) that records verified source call chains. Structure:

- Hash routing: `#/` is the home page (post cards, live search, tag filter, stats, about/rules); `#/post/<id>` is the article view (flow overview, section TOC — right sidebar on wide screens, bookmark tables, prev/next nav, reading progress bar). A persistent left site-nav (all posts, active highlight, auto-generated from `POSTS`) is visible on wide screens. Legacy `#tab-*` hashes redirect to the corresponding post.
- Content is data-driven: each investigation is one object in the `POSTS` array near the top of the `<script>` — `id`, `title`, `date`, `tags`, `excerpt`, `lead`, `flow` (colored overview diagram), `sections` (name, thread chip, keynote, bookmark rows), `notes`. All views are rendered from this data.
- Existing posts: `render-command-chain` (ENQUEUE_RENDER_COMMAND -> TGraphTask -> lock-free queue -> RenderThread) and `init-resource-chain` (`Resource->InitResource()` end to end, from the TPViewport trigger to `FHZBOcclusionTester::InitDynamicRHI`, plus Update/Release lifecycle).

Recording rule for future investigations:

- A new verified investigation becomes a new `POSTS` entry; follow-up findings append rows to that post's sections or notes. Do not scatter these records into separate files.
- Row format is fixed: symbol, copyable `absolute\path\file:line` (the 复制 button copies exactly that token, VS Ctrl+G friendly), and a role note covering control flow / data ownership / thread.
- Mark what is verified in source versus inferred or unverified runtime behavior; note debugger-stack line drift when relevant.
- Light/dark theme toggle is stored in localStorage; keep everything static and dependency-free.

## Verification

For web-note changes:

- Open `http://127.0.0.1:4177/` from `docs/render-notes-site/`
- Check learning-route navigation, module jumps, search, and browser console errors

For UE code changes:

- Prefer a targeted build or editor repro when feasible
- If a full UE build is too heavy, state exactly what static checks were done
