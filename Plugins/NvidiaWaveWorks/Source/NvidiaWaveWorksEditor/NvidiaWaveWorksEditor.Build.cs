using UnrealBuildTool;

public class NvidiaWaveWorksEditor : ModuleRules
{
	public NvidiaWaveWorksEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"Slate",
				"SlateCore",
				"UnrealEd",
				"AssetDefinition",
				"EditorFramework",
				"AdvancedPreviewScene",
				"PropertyEditor",
				"ToolMenus",
				"ProceduralMeshComponent",
				"NvidiaWaveWorks"
			});
	}
}
