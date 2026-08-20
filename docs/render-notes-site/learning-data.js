window.UE_LEARNING_SITE = {
  modules: [
    {
      id: "orientation",
      title: "源码阅读入口",
      short: "先建立地图，再进入渲染细节",
      level: "入门",
      time: "0.5 天",
      summary: "不要从某个类直接硬读。先分清 Game Thread、Render Thread、RHI Thread、GPU 之间的责任，再把每个系统放回整条帧管线。",
      questions: [
        "一帧从 World Tick 到 GPU command list 经过哪些线程边界？",
        "Renderer、RenderCore、RHI、D3D12RHI 分别负责什么？",
        "源码阅读时应该先看调用链、数据结构还是 shader 文件？"
      ],
      files: [
        "Engine/Source/Runtime/Renderer/Private/SceneRendering.cpp",
        "Engine/Source/Runtime/RenderCore/Public/RenderingThread.h",
        "Engine/Source/Runtime/RHI/Public/RHICommandList.h",
        "Engine/Source/Runtime/D3D12RHI/Private/D3D12Commands.cpp"
      ],
      callPath: [
        "游戏逻辑产生场景状态",
        "Renderer 构建 FSceneRenderer",
        "Render Thread 组织 pass 和资源",
        "RHI 翻译为平台命令",
        "D3D12RHI 提交 command list"
      ],
      labs: [
        "在 Render Thread 入口加日志，确认当前线程和 view family 数量",
        "找一个控制台变量，追踪它如何影响渲染分支"
      ],
      pitfalls: [
        "把 Renderer 和 RHI 混成一层，会导致调用路径看不清",
        "只看类名不看线程边界，容易误判对象生命周期"
      ],
      tags: ["Frame", "Thread", "Renderer", "RHI"]
    },
    {
      id: "material",
      title: "材质到 Shader",
      short: "从材质图到 HLSL 输入",
      level: "基础",
      time: "1 天",
      summary: "材质系统的重点不是节点 UI，而是材质表达式如何生成 HLSL、如何参与 shader permutation，以及最终如何进入 BasePass shader。",
      questions: [
        "材质图中的节点如何被翻译成 HLSL 片段？",
        "Material Instance 修改参数后，哪些东西需要重新编译？",
        "BasePass 为什么可以拿到材质属性？"
      ],
      files: [
        "Engine/Source/Runtime/Engine/Private/Materials/Material.cpp",
        "Engine/Source/Runtime/Engine/Private/Materials/HLSLMaterialTranslator.cpp",
        "Engine/Source/Runtime/Renderer/Private/BasePassRendering.cpp",
        "Engine/Shaders/Private/BasePassPixelShader.usf"
      ],
      callPath: [
        "UMaterial / UMaterialInstance 收集表达式和参数",
        "FMaterial 生成编译环境",
        "HLSLMaterialTranslator 输出材质代码",
        "BasePass shader 读取材质属性并写入 GBuffer"
      ],
      labs: [
        "创建一个简单材质，观察生成 HLSL 中 BaseColor / Roughness 的来源",
        "修改一个静态开关，确认 shader map 是否变化"
      ],
      pitfalls: [
        "动态材质参数不等于 shader 重新编译",
        "材质图结果和最终像素颜色之间还隔着光照、后处理和色调映射"
      ],
      tags: ["Material", "HLSL", "BasePass"]
    },
    {
      id: "shader-compile",
      title: "Shader 编译系统",
      short: "Job、Worker、DDC 与 ShaderMap",
      level: "核心",
      time: "2 天",
      summary: "Shader 编译是理解 UE 渲染源码的第一条硬路径。它连接材质、全局 shader、平台编译器、DDC 缓存和运行时 RHI shader。",
      questions: [
        "FShaderCompileJob 是在哪里创建和排队的？",
        "ShaderCompileWorker 和主进程如何交换输入输出？",
        "编译结果如何回填到 FShaderMap 并创建 RHI shader？"
      ],
      files: [
        "Engine/Source/Runtime/Engine/Private/ShaderCompiler/ShaderCompiler.cpp",
        "Engine/Source/Programs/ShaderCompileWorker/Private/ShaderCompileWorker.cpp",
        "Engine/Source/Runtime/RenderCore/Public/Shader.h",
        "Engine/Source/Runtime/D3D12RHI/Private/D3D12Shaders.cpp"
      ],
      callPath: [
        "创建 FShaderCompileJob / FShaderCommonCompileJob",
        "FShaderCompilingManager 管理队列和优先级",
        "Worker 进程读取输入文件并调用平台编译器",
        "主进程收集结果，填充 FShaderMap",
        "运行时通过 TShaderRef 取到 FRHIShader"
      ],
      labs: [
        "在 ShaderCompileWorker 启动处打断点，确认命令行参数和工作目录",
        "故意写错一个 usf，观察错误如何回传到编辑器"
      ],
      pitfalls: [
        "不要把编译 job 和最终 RHI shader 当成同一个对象",
        "DDC 命中会让很多编译路径被跳过，调试前要确认缓存状态"
      ],
      tags: ["ShaderCompileWorker", "DDC", "ShaderMap", "RHIShader"]
    },
    {
      id: "rdg",
      title: "RDG 渲染依赖图",
      short: "Pass、资源生命周期与自动屏障",
      level: "核心",
      time: "2 天",
      summary: "RDG 是现代 UE 渲染代码的组织方式。重点是 pass 声明读写资源，RDG 负责裁剪、排序、生命周期和 barrier，而不是把命令立即执行。",
      questions: [
        "AddPass 到底记录了什么？什么时候真正执行？",
        "FRDGTexture 和真实 RHI texture 的关系是什么？",
        "为什么 RDG 能自动插入资源状态转换？"
      ],
      files: [
        "Engine/Source/Runtime/RenderCore/Public/RenderGraphBuilder.h",
        "Engine/Source/Runtime/RenderCore/Private/RenderGraphBuilder.cpp",
        "Engine/Source/Runtime/RenderCore/Private/RenderGraphPass.cpp",
        "Engine/Source/Runtime/Renderer/Private/SceneRendering.cpp"
      ],
      callPath: [
        "创建 FRDGBuilder",
        "注册 texture / buffer / external resource",
        "AddPass 声明参数和执行 lambda",
        "Compile 阶段分析依赖和资源生命周期",
        "Execute 阶段发出 RHI command"
      ],
      labs: [
        "写一个最小 RDG pass 清屏或拷贝纹理",
        "打开 RDG 调试输出，观察 pass 是否被裁剪"
      ],
      pitfalls: [
        "在 AddPass 外访问 RDG 临时资源通常是生命周期错误",
        "Pass lambda 里捕获对象要确认线程和生命周期"
      ],
      tags: ["RDG", "Pass", "Barrier", "Lifetime"]
    },
    {
      id: "basepass",
      title: "BasePass 与 GBuffer",
      short: "场景几何如何写入延迟渲染输入",
      level: "核心",
      time: "1.5 天",
      summary: "BasePass 是延迟渲染的关键入口。它把 mesh、material、view、lightmap 等信息组合起来，输出后续光照阶段需要的 GBuffer。",
      questions: [
        "Mesh draw command 是如何选到 BasePass shader 的？",
        "GBuffer 每个 render target 大致承载哪些数据？",
        "Forward 和 Deferred 下 BasePass 的目标有什么不同？"
      ],
      files: [
        "Engine/Source/Runtime/Renderer/Private/BasePassRendering.cpp",
        "Engine/Source/Runtime/Renderer/Private/MeshPassProcessor.cpp",
        "Engine/Source/Runtime/Renderer/Private/SceneVisibility.cpp",
        "Engine/Shaders/Private/BasePassPixelShader.usf"
      ],
      callPath: [
        "Visibility 阶段筛选 primitive",
        "MeshPassProcessor 生成 draw command",
        "BasePass shader 评估材质",
        "输出 SceneColor / Depth / GBuffer",
        "后续 Lighting pass 读取 GBuffer"
      ],
      labs: [
        "切换 Buffer Visualization，验证 GBuffer 内容",
        "跟踪一个 StaticMesh 从 visibility 到 draw command 的路径"
      ],
      pitfalls: [
        "BasePass 不等于最终光照结果",
        "不同 shading model 会改变 GBuffer 编码和后续解码"
      ],
      tags: ["BasePass", "GBuffer", "MeshDrawCommand"]
    },
    {
      id: "lighting",
      title: "光照、阴影与后处理",
      short: "从 GBuffer 到最终 SceneColor",
      level: "进阶",
      time: "3 天",
      summary: "这部分把渲染结果从几何属性推进到最终画面。建议先看直接光和阴影，再看 Lumen、Nanite、TSR 这类大系统。",
      questions: [
        "Deferred lighting 如何读取 GBuffer 并累加到 SceneColor？",
        "Shadow map / VSM 的生产和消费分别在哪里？",
        "后处理链路如何处理 Bloom、ToneMap、TSR？"
      ],
      files: [
        "Engine/Source/Runtime/Renderer/Private/LightRendering.cpp",
        "Engine/Source/Runtime/Renderer/Private/ShadowRendering.cpp",
        "Engine/Source/Runtime/Renderer/Private/PostProcess/PostProcessing.cpp",
        "Engine/Shaders/Private/DeferredLightPixelShaders.usf"
      ],
      callPath: [
        "光源收集和可见性判断",
        "生成或复用阴影资源",
        "Deferred light pass 根据 GBuffer 计算光照",
        "PostProcess 链路处理 SceneColor",
        "ToneMap / TSR 输出显示目标"
      ],
      labs: [
        "关闭某类光源或阴影，观察 pass 列表变化",
        "用 RenderDoc 捕获一帧，定位 DeferredLight pass"
      ],
      pitfalls: [
        "Lumen / Nanite / TSR 都是大系统，不适合作为第一站",
        "后处理看到的颜色空间可能已经不是线性 HDR 原始值"
      ],
      tags: ["Lighting", "Shadow", "PostProcess", "TSR"]
    },
    {
      id: "rhi-d3d12",
      title: "RHI 到 D3D12",
      short: "UE 抽象如何落到平台 API",
      level: "深入",
      time: "3 天",
      summary: "RHI 层把 UE 渲染命令隔离成平台无关接口。学习 D3D12RHI 时，要重点看 resource、descriptor、command list、PSO 和 root signature。",
      questions: [
        "FRHICommandList 的命令何时被翻译为 FD3D12CommandList？",
        "UE 如何管理 descriptor heap 和资源状态？",
        "PSO / RootSignature 的缓存和绑定在哪里发生？"
      ],
      files: [
        "Engine/Source/Runtime/RHI/Public/RHICommandList.h",
        "Engine/Source/Runtime/D3D12RHI/Private/D3D12Commands.cpp",
        "Engine/Source/Runtime/D3D12RHI/Private/D3D12State.cpp",
        "Engine/Source/Runtime/D3D12RHI/Private/D3D12RootSignature.cpp"
      ],
      callPath: [
        "Renderer / RDG 发出 RHI 命令",
        "FRHICommandList 记录或立即执行",
        "D3D12RHI 转换为 D3D12 command list",
        "绑定 PSO、root signature、descriptor",
        "提交到 command queue"
      ],
      labs: [
        "在 SetGraphicsPipelineState 路径打断点，观察 PSO 初始化",
        "跟踪一个 shader resource view 的 descriptor 分配"
      ],
      pitfalls: [
        "RHI 对象不一定马上拥有底层 D3D12 对象",
        "资源状态可能由 RDG 和 RHI 两层共同影响，不能只看一处"
      ],
      tags: ["RHI", "D3D12", "PSO", "RootSignature"]
    },
    {
      id: "debugging",
      title: "调试方法与证据链",
      short: "用断点、日志、RenderDoc 建证据",
      level: "方法",
      time: "持续",
      summary: "学习源码不能只记概念。每个主题都应该落到一个可复现的实验：断点在哪里、变量看什么、RenderDoc 捕获哪一帧、预期现象是什么。",
      questions: [
        "一个渲染问题应该从 CPU 侧还是 GPU 捕获开始？",
        "如何确认某个 pass 真的执行了？",
        "如何区分 shader 编译问题、资源绑定问题和渲染逻辑问题？"
      ],
      files: [
        "Engine/Source/Runtime/Renderer/Private/SceneRendering.cpp",
        "Engine/Source/Runtime/RenderCore/Private/RenderGraphBuilder.cpp",
        "Engine/Source/Runtime/D3D12RHI/Private/D3D12Commands.cpp",
        "Plugins/TPViewport/Source/TPViewport/Private/D3D12IntegrationExample.cpp"
      ],
      callPath: [
        "先定义预期画面或状态",
        "确认 CPU 侧是否走到目标路径",
        "确认 RDG / RHI pass 是否存在",
        "捕获 GPU 帧验证资源和 shader",
        "只修改最小可疑路径"
      ],
      labs: [
        "给当前 TPViewport 插件建立一个最小 RHI 实验页",
        "用一帧捕获证明某个自定义 shader 的输入输出"
      ],
      pitfalls: [
        "没有预期值的日志很难证明问题",
        "看到画面错不代表 shader 错，可能是资源、状态或时序错"
      ],
      tags: ["Debug", "RenderDoc", "Evidence", "Plugin"]
    }
  ],
  routes: [
    {
      title: "路线 A：先会读源码",
      audience: "适合刚开始看 UE 渲染源码",
      steps: ["源码阅读入口", "Shader 编译系统", "RDG 渲染依赖图", "BasePass 与 GBuffer"],
      outcome: "能把一条渲染调用链从 CPU 侧追到 shader 文件。"
    },
    {
      title: "路线 B：先做 RHI 实验",
      audience: "适合当前这个 UE_PRG_RHI_Study 项目",
      steps: ["源码阅读入口", "RHI 到 D3D12", "Shader 编译系统", "调试方法与证据链"],
      outcome: "能解释一个自定义 RHI/Shader 实验从 C++ 到 D3D12 的绑定路径。"
    },
    {
      title: "路线 C：先理解画面",
      audience: "适合从渲染效果倒推源码",
      steps: ["材质到 Shader", "BasePass 与 GBuffer", "光照、阴影与后处理", "RDG 渲染依赖图"],
      outcome: "能从一个像素结果反推材质、GBuffer、光照和后处理阶段。"
    }
  ],
  sourceAreas: [
    {
      area: "Renderer",
      role: "组织场景渲染、mesh pass、光照、阴影、后处理，是阅读 UE 渲染源码的主战场。",
      paths: [
        "Engine/Source/Runtime/Renderer/Private/SceneRendering.cpp",
        "Engine/Source/Runtime/Renderer/Private/BasePassRendering.cpp",
        "Engine/Source/Runtime/Renderer/Private/LightRendering.cpp"
      ]
    },
    {
      area: "RenderCore",
      role: "提供 shader、RDG、渲染线程、资源抽象等跨 Renderer 和 RHI 的基础设施。",
      paths: [
        "Engine/Source/Runtime/RenderCore/Public/RenderGraphBuilder.h",
        "Engine/Source/Runtime/RenderCore/Public/Shader.h",
        "Engine/Source/Runtime/RenderCore/Public/RenderingThread.h"
      ]
    },
    {
      area: "RHI",
      role: "平台无关渲染硬件接口，定义 command list、resource、pipeline state 等抽象。",
      paths: [
        "Engine/Source/Runtime/RHI/Public/RHI.h",
        "Engine/Source/Runtime/RHI/Public/RHICommandList.h",
        "Engine/Source/Runtime/RHI/Public/RHIPipeline.h"
      ]
    },
    {
      area: "D3D12RHI",
      role: "Windows/D3D12 平台实现，适合研究 command list、descriptor、PSO、root signature。",
      paths: [
        "Engine/Source/Runtime/D3D12RHI/Private/D3D12Commands.cpp",
        "Engine/Source/Runtime/D3D12RHI/Private/D3D12State.cpp",
        "Engine/Source/Runtime/D3D12RHI/Private/D3D12RootSignature.cpp"
      ]
    },
    {
      area: "Shaders",
      role: "USF/USH 文件承载 GPU 侧逻辑，需要和 C++ permutation、参数结构一起读。",
      paths: [
        "Engine/Shaders/Private/BasePassPixelShader.usf",
        "Engine/Shaders/Private/DeferredLightPixelShaders.usf",
        "Engine/Shaders/Private/PostProcessTonemap.usf"
      ]
    },
    {
      area: "当前项目插件",
      role: "本项目自己的 RHI/Viewport 实验区，适合把源码学习落到可运行验证。",
      paths: [
        "Plugins/TPViewport/Source/TPViewport/",
        "Plugins/TPViewport/Shaders/Private/SimpleShader.USF",
        "Plugins/D3D12RHIAccess/Source/D3D12RHIAccess/"
      ]
    }
  ],
  labs: [
    {
      title: "实验 1：证明一个自定义 shader 被编译",
      target: "Shader 编译系统",
      checks: [
        "修改 SimpleShader.USF 产生可见变化",
        "观察编译日志或断点命中",
        "确认运行时使用的 shader map 已更新"
      ]
    },
    {
      title: "实验 2：写一个最小 RDG Pass",
      target: "RDG 渲染依赖图",
      checks: [
        "AddPass 中声明输入输出资源",
        "用 RDG 调试工具确认 pass 存在",
        "用 RenderDoc 验证 GPU 事件和目标纹理"
      ]
    },
    {
      title: "实验 3：追踪 PSO 与 RootSignature",
      target: "RHI 到 D3D12",
      checks: [
        "从 SetGraphicsPipelineState 进入 D3D12RHI",
        "记录 bound shader、input layout、render target format",
        "确认 root signature 来自 shader binding layout"
      ]
    },
    {
      title: "实验 4：从像素倒查 BasePass",
      target: "BasePass 与 GBuffer",
      checks: [
        "打开 GBuffer 可视化",
        "定位 BasePass draw event",
        "对照材质参数和 GBuffer 编码"
      ]
    }
  ],
  terms: [
    ["RDG", "Render Dependency Graph。用声明式 pass 和资源依赖管理现代渲染流程。"],
    ["RHI", "Render Hardware Interface。UE 对平台图形 API 的抽象层。"],
    ["ShaderMap", "一组已编译 shader 的集合，通常由材质、平台、permutation 决定。"],
    ["GBuffer", "延迟渲染中保存材质和几何属性的一组 render target。"],
    ["PSO", "Pipeline State Object。D3D12/Vulkan 等现代 API 中描述固定管线和 shader 组合的对象。"],
    ["RootSignature", "D3D12 中定义 shader 如何访问资源绑定槽位的布局。"],
    ["MeshDrawCommand", "UE 为 mesh pass 缓存的绘制命令，连接 mesh、material、shader 和 pipeline state。"],
    ["ShaderCompileWorker", "UE 独立 shader 编译进程，主进程通过任务和文件 IPC 获取编译结果。"],
    ["DDC", "Derived Data Cache。缓存 shader、纹理等派生数据，影响调试时路径是否真正执行。"]
  ]
};
