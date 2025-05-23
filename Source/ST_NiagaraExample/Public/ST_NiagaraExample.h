// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class ST_NIAGARAEXAMPLE_API FST_NiagaraExampleModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FST_NiagaraExampleModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FST_NiagaraExampleModule>("ST_NiagaraExample");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ST_NiagaraExample");
	}
};