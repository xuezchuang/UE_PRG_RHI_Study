using System.IO;
using UnrealBuildTool;

public class HBAOPlusSDK : ModuleRules
{
	public HBAOPlusSDK(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			string HBAOPlusRoot = Path.GetFullPath(
				Path.Combine(ModuleDirectory, "..", "..", "..", "ThirdParty", "HBAOPlus"));
			string IncludePath = Path.Combine(HBAOPlusRoot, "Include");
			string ImportLibraryPath = Path.Combine(
				HBAOPlusRoot,
				"Lib",
				"Win64",
				"GFSDK_SSAO_D3D12.win64.lib");
			string DllPath = Path.Combine(
				HBAOPlusRoot,
				"Bin",
				"Win64",
				"GFSDK_SSAO_D3D12.win64.dll");

			PublicSystemIncludePaths.Add(IncludePath);
			PublicAdditionalLibraries.Add(ImportLibraryPath);
			PublicDelayLoadDLLs.Add("GFSDK_SSAO_D3D12.win64.dll");
			RuntimeDependencies.Add(
				"$(PluginDir)/Binaries/ThirdParty/HBAOPlus/Win64/GFSDK_SSAO_D3D12.win64.dll",
				DllPath);
			PublicDefinitions.Add("GFSDK_SSAO_DYNAMIC_LOAD_LIBRARY=0");
			PublicDefinitions.Add("WITH_NVIDIA_HBAOPLUS=1");
		}
		else
		{
			PublicDefinitions.Add("WITH_NVIDIA_HBAOPLUS=0");
		}
	}
}
