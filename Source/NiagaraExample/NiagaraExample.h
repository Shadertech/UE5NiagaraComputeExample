// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class NIAGARAEXAMPLE_API FNiagaraExampleModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FNiagaraExampleModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FNiagaraExampleModule>("NiagaraExample");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("NiagaraExample");
	}
};