// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class COMPUTERPEXAMPLE_API FComputeRPExampleModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FComputeRPExampleModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FComputeRPExampleModule>("ComputeRPExample");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ComputeRPExample");
	}
};