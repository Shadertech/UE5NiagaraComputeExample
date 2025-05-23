// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

using UnrealBuildTool;

public class ST_ComputeCore : ModuleRules
{
	public ST_ComputeCore(ReadOnlyTargetRules Target) : base(Target)
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
                "RenderCore",
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
                "Niagara",
				"NiagaraCore",
				"VectorVM",
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
