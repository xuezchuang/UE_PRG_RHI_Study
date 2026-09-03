using UnrealBuildTool;

public class NvidiaHBAOPlusEditor : ModuleRules
{
	public NvidiaHBAOPlusEditor(ReadOnlyTargetRules Target) : base(Target)
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
				"NvidiaHBAOPlus"
			});
	}
}
