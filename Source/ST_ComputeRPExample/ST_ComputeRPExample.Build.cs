// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

using UnrealBuildTool;

public class ST_ComputeRPExample : ModuleRules
{
	public ST_ComputeRPExample(ReadOnlyTargetRules Target) : base(Target)
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
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "ST_ComputeCore",
                "RenderCore",
                "DeveloperSettings"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
                "Renderer",
                "RenderCore",
                "RHI",
                "Projects",
                "Niagara"
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
