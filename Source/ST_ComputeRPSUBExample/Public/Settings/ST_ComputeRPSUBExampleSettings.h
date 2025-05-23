// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "NiagaraSystem.h"
#include "Data/ST_BoidsLib.h"
#include "ST_ComputeRPSUBExampleSettings.generated.h"

UCLASS(Config = ST_NiagaraCSIntegration, DefaultConfig, meta = (DisplayName = "ST Compute RP SUB Example Settings"))
class ST_COMPUTERPSUBEXAMPLE_API UST_ComputeRPSUBExampleSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "ShaderTech|ComputeRPSUBExampleSettings")
	static const UST_ComputeRPSUBExampleSettings* GetComputeRPSUBExampleSettings()
	{
		return GetDefault<UST_ComputeRPSUBExampleSettings>();
	}

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Boids")
	TSoftObjectPtr<UNiagaraSystem> BoidsEmitterMaterialVFX = nullptr;

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Boids")
	FST_BoidConstantParameters BoidConstantParameters;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Boids")
	FST_BoidDynamicParameters BoidDynamicParameters;

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Material")
	TSoftObjectPtr<UMaterialInterface> BoidsMatInterface = nullptr;

protected:

	// UDeveloperSettings interface.
	virtual FName GetCategoryName() const override
	{
		return TEXT("ShaderTech");
	}
};