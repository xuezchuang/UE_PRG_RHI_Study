# NVIDIA HBAO+ UE 5.8 Plugin

这个项目级插件通过 NVIDIA HBAO+ 4.0 的 D3D12 接口渲染屏幕空间环境光遮蔽，不修改 Unreal Engine。

## 使用

1. 使用 D3D12 启动 `RPG.uproject`。
2. 在 Content Browser 中选择 `Add -> Rendering -> NVIDIA HBAO+`。
3. 新建并双击资产。
4. 左侧 Slate Viewport 显示用于观察接触阴影和遮蔽半径的预览场景。
5. 右侧 Details 可实时调整 Radius、Bias、大小尺度 AO、质量和模糊参数。
6. 工具栏可启用/禁用 HBAO+、显示原始 AO，并重建原生 D3D12 Context。

开发版本提供以下控制台命令：

```text
NvidiaHBAOPlus.Status
NvidiaHBAOPlus.ResetContext
```

## 当前渲染路径

- Runtime 模块注册 UE 的 Post-Opaque RDG 扩展。
- `D3D12RHIAccess` 提供当前 UE D3D12 Device、Graphics Queue 和 Command List。
- SceneDepth 由 RDG 转换为图形 SRV 状态，并通过 HBAO+ 专用 shader-visible descriptor heap 传给 SDK。
- HBAO+ 使用 `RenderAOMultiFrame` 在 UE 当前 command list 中记录绘制。
- 普通模式以 `GFSDK_SSAO_MULTIPLY_RGB` 合成到不透明 SceneColor；Visualize AO 模式覆盖显示原始 AO。
- 调用结束后使 UE RHI 缓存状态失效，避免 NVIDIA 原生 PSO、Root Signature 和 descriptor heap 污染后续 UE 绘制。

当前版本是纯插件集成，效果位于不透明光照完成后的 Post-Opaque 阶段。它没有替换 Engine 内部的 `SceneTextures.ScreenSpaceAO`；因此不会重新参与已经完成的 Deferred Lighting。

## 平台与许可

- 当前仅支持 Win64 + D3D12。
- 为了在 SDK 原生绘制后清理 UE 的 D3D12 图形状态缓存，当前实现使用 UE 5.8 的 D3D12RHI 私有接口；升级 Engine 版本时需要重新编译并复核这一层。
- 第一版由 HBAO+ 从 SceneDepth 重建法线，暂不读取 UE GBuffer Normal。
- SDK 文件受 `ThirdParty/HBAOPlus/LICENSE.txt` 中 NVIDIA Source Code License 约束。
- 分发插件时必须保留完整许可证和原有版权、专利、商标与归属声明。
