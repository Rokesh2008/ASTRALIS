using UnrealBuildTool;

public class game1Target : TargetRules
{
	public game1Target(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange(new string[] { "Astralis", "AstralisSimulation", "AstralisData" });
	}
}
