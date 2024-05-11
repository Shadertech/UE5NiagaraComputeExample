// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Core/ManagedGBCSInterface.h"

class COMPUTEGBEXAMPLE_API FComputeGBExampleModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void Register(IManagedGBCSInterface* ManagedCS);
	void Deregister(IManagedGBCSInterface* ManagedCS);

	TArray<IManagedGBCSInterface*> ManagedCSs;

	static inline FComputeGBExampleModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FComputeGBExampleModule>("ComputeGBExample");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ComputeGBExample");
	}

private:
	void HandlePreRender_RenderThread(class FRDGBuilder& RDGBuilder);

	void BeginRendering();
	void EndRendering();
};