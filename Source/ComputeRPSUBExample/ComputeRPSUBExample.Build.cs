// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

using UnrealBuildTool;

public class ComputeRPSUBExample : ModuleRules
{
	public ComputeRPSUBExample(ReadOnlyTargetRules Target) : base(Target)
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
                "ComputeCore",
                "ComputeRPExample",
				// ... add other public dependencies that you statically link with here ...
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
				"ComputeCore",
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
