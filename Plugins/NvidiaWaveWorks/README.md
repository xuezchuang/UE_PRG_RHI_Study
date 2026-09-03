# NVIDIA WaveWorks UE 5.8 Plugin

这个插件把 NVIDIA WaveWorks 2.x 作为项目级 UE 插件接入，不修改 Engine。

## 使用

1. 使用 D3D12 启动 `RPG.uproject`。
2. 在 Content Browser 中选择 `Add -> Rendering -> NVIDIA WaveWorks`。
3. 新建并双击资产。
4. 左侧 Slate Viewport 显示 UE 渲染的海面预览；右侧 Details 可实时调整风、涌浪、表面和泡沫参数。
5. 工具栏可暂停或重置模拟，Viewport 左下角显示 WaveWorks 运行状态。

开发版本还提供控制台命令：

```text
NvidiaWaveWorks.SmokeTest
```

该命令会验证 UE D3D12 device/queue、WaveWorks GPU 模拟和 CPU 位移回读。

## 当前渲染路径

- WaveWorks 使用 UE 暴露的原生 `ID3D12Device` 和 graphics queue 执行计算。
- 位移通过 WaveWorks readback API 异步返回 CPU。
- UE 使用 `UProceduralMeshComponent` 更新并渲染预览网格。
- Slate、Details、资产保存和 Preview Scene 均由 UE 管理。

这是第一版可工作的学习/预览路径。它还没有把 WaveWorks 原生 displacement/normal/foam texture 直接注册进 RDG，也没有替换 UE 主渲染器或 Water 系统。

## 平台与许可

- 当前仅支持 Win64 + D3D12。
- SDK 头文件、导入库和 DLL 仍受 NVIDIA GameWorks SDK EULA 约束，详见 `ThirdPartyNotices.txt`。
- 在确认再分发权限前，建议仓库保持 private。
