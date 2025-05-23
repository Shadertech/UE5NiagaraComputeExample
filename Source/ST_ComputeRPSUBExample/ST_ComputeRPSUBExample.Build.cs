// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

using UnrealBuildTool;

public class ST_ComputeRPSUBExample : ModuleRules
{
	public ST_ComputeRPSUBExample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
                ModuleDirectory,
                "Runtime/Renderer/Private"
            }
            );
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "RenderCore",
                "ST_ComputeCore",
                "ST_ComputeRPExample",
                "DeveloperSettings"
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Engine",
                "CoreUObject",
                "Renderer",
                "RenderCore",
                "RHI",
                "Projects",
                "Engine",
                "Niagara",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
    }
}
