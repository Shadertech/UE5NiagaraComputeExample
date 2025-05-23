// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class ST_COMPUTERPEXAMPLE_API FST_ComputeRPExampleModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FST_ComputeRPExampleModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FST_ComputeRPExampleModule>("ST_ComputeRPExample");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ST_ComputeRPExample");
	}
};