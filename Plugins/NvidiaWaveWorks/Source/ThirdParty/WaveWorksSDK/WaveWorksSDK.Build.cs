using System.IO;
using UnrealBuildTool;

public class WaveWorksSDK : ModuleRules
{
	public WaveWorksSDK(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			string WaveWorksRoot = Path.GetFullPath(
				Path.Combine(ModuleDirectory, "..", "..", "..", "ThirdParty", "WaveWorks"));
			string IncludePath = Path.Combine(WaveWorksRoot, "Include");
			string ImportLibraryPath = Path.Combine(WaveWorksRoot, "Lib", "Win64", "NVWaveWorks_shared.lib");
			string DllPath = Path.Combine(WaveWorksRoot, "Bin", "Win64", "NVWaveWorks_shared.dll");

			PublicSystemIncludePaths.Add(IncludePath);
			PublicAdditionalLibraries.Add(ImportLibraryPath);
			PublicDelayLoadDLLs.Add("NVWaveWorks_shared.dll");
			RuntimeDependencies.Add(
				"$(PluginDir)/Binaries/ThirdParty/WaveWorks/Win64/NVWaveWorks_shared.dll",
				DllPath);
			PublicDefinitions.Add("WAVEWORKS_DYNAMIC_BUILD=1");
			PublicDefinitions.Add("WITH_NVIDIA_WAVEWORKS=1");
		}
		else
		{
			PublicDefinitions.Add("WITH_NVIDIA_WAVEWORKS=0");
		}
	}
}
