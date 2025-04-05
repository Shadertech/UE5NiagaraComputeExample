// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class COMPUTERPLEGACYEXAMPLE_API FComputeRPLegacyExampleModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FComputeRPLegacyExampleModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FComputeRPLegacyExampleModule>("ComputeRPLegacyExample");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ComputeRPLegacyExample");
	}
};