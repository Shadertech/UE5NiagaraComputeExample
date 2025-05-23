// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

using UnrealBuildTool;

public class ST_ComputeRPLegacyExample : ModuleRules
{
	public ST_ComputeRPLegacyExample(ReadOnlyTargetRules Target) : base(Target)
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
                "ST_ComputeRPExample",
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
