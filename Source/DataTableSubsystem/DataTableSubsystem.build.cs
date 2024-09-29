using UnrealBuildTool;
 
public class DataTableSubsystem : ModuleRules
{
	public DataTableSubsystem(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine"});
 
		PublicIncludePaths.AddRange(new string[] {"DataTableSubsystem/Public"});
		PrivateIncludePaths.AddRange(new string[] {"DataTableSubsystem/Private"});

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUtils",
				// ... add private dependencies that you statically link with here ...	
			}
            );        

    }
}