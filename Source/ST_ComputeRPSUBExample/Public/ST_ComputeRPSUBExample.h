// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class ST_COMPUTERPSUBEXAMPLE_API FST_ComputeRPSUBExampleModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FST_ComputeRPSUBExampleModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FST_ComputeRPSUBExampleModule>("ST_ComputeRPSUBExample");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ST_ComputeRPSUBExample");
	}
};
