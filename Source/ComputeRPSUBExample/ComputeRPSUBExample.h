// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class COMPUTERPSUBEXAMPLE_API FComputeRPSUBExampleModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FComputeRPSUBExampleModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FComputeRPSUBExampleModule>("ComputeRPSUBExample");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ComputeRPSUBExample");
	}
};
