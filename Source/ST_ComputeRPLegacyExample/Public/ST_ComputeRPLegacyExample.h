// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class ST_COMPUTERPLEGACYEXAMPLE_API FST_ComputeRPLegacyExampleModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FST_ComputeRPLegacyExampleModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FST_ComputeRPLegacyExampleModule>("ST_ComputeRPLegacyExample");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ST_ComputeRPLegacyExample");
	}
};