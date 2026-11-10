using UnrealBuildTool;

public class AstralisSimulation : ModuleRules
{
	public AstralisSimulation(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"AstralisData"
		});
	}
}
