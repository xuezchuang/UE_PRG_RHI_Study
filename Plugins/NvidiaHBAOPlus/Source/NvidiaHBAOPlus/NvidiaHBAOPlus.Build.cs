using System.IO;
using UnrealBuildTool;

public class NvidiaHBAOPlus : ModuleRules
{
	public NvidiaHBAOPlus(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine"
			});

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"Projects",
				"RenderCore",
				"Renderer",
				"RHI",
				"RHICore",
				"TraceLog",
				"D3D12RHIAccess",
				"HBAOPlusSDK"
			});

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PrivateDependencyModuleNames.AddRange(
				new[]
				{
					"D3D12RHI"
				});
			PrivateIncludePaths.Add(
				Path.Combine(EngineDirectory, "Source/Runtime/D3D12RHI/Private"));
			PrivateIncludePaths.Add(
				Path.Combine(EngineDirectory, "Source/Runtime/RHICore/Internal"));
			AddEngineThirdPartyPrivateStaticDependencies(
				Target,
				"AMD_AGS",
				"IntelExtensionsFramework",
				"NVAftermath",
				"NVAPI");
			AddEngineThirdPartyPrivateStaticDependencies(Target, "DX12");
		}
	}
}
