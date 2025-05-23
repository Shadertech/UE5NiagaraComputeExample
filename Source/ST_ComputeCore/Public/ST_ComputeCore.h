// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class ST_COMPUTECORE_API FST_ComputeCoreModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FST_ComputeCoreModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FST_ComputeCoreModule>("ST_ComputeCore");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ST_ComputeCore");
	}
};