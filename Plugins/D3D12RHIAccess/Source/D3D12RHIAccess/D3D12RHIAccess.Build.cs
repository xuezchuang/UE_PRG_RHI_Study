// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class D3D12RHIAccess : ModuleRules
{
	public D3D12RHIAccess(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"RHI",
				"RenderCore"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects"
			}
		);

		// Add D3D12RHI module dependency for Windows platform
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicDependencyModuleNames.Add("D3D12RHI");
			
			// Add Windows SDK includes
			PublicSystemIncludePaths.Add("$(WindowsSdkDir)/Include/$(WindowsSDKVersion)/um");
			PublicSystemIncludePaths.Add("$(WindowsSdkDir)/Include/$(WindowsSDKVersion)/shared");
			
			// Add D3D12 libraries
			PublicSystemLibraries.Add("d3d12.lib");
			PublicSystemLibraries.Add("dxgi.lib");
			
			// Add preprocessor definition to enable D3D12 code paths
			PublicDefinitions.Add("WITH_D3D12_RHI=1");
		}
		else
		{
			PublicDefinitions.Add("WITH_D3D12_RHI=0");
		}
	}
}
