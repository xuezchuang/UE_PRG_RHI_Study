using UnrealBuildTool;

public class NvidiaWaveWorks : ModuleRules
{
	public NvidiaWaveWorks(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"ProceduralMeshComponent"
			});

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"Projects",
				"RenderCore",
				"RHI",
				"WaveWorksSDK"
			});

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PrivateDependencyModuleNames.Add("D3D12RHI");
			AddEngineThirdPartyPrivateStaticDependencies(Target, "DX12");
		}
	}
}
