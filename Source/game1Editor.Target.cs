using UnrealBuildTool;

public class game1EditorTarget : TargetRules
{
	public game1EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange(new string[] { "Astralis", "AstralisSimulation", "AstralisData" });
	}
}
