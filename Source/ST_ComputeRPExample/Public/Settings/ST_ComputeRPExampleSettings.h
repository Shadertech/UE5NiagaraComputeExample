// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "NiagaraSystem.h"
#include "Data/ST_BoidsLib.h"
#include "ST_ComputeRPExampleSettings.generated.h"

UCLASS(Config = ST_NiagaraCSIntegration, DefaultConfig, meta = (DisplayName = "ST Compute RP Example Settings"))
class ST_COMPUTERPEXAMPLE_API UST_ComputeRPExampleSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "ShaderTech|ComputeRPExampleSettings")
	static const UST_ComputeRPExampleSettings* GetComputeRPExampleSettings()
	{
		return GetDefault<UST_ComputeRPExampleSettings>();
	}

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Boids")
	TSoftObjectPtr<UNiagaraSystem> BoidsEmitterVFX = nullptr;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Boids")
	TSoftObjectPtr<UNiagaraSystem> BoidsDrawerVFX = nullptr;

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Boids")
	FST_BoidConstantParameters BoidEmitterConstantParameters;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Boids")
	FST_BoidConstantParameters BoidDrawerConstantParameters;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Boids")
	FST_BoidDynamicParameters BoidDynamicParameters;

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Material")
	TSoftObjectPtr<UMaterialInterface> BoidsMatInterface = nullptr;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Material")
	TSoftObjectPtr<UMaterialInterface> BoidsDrawMatInterface = nullptr;

protected:

	// UDeveloperSettings interface.
	virtual FName GetCategoryName() const override
	{
		return TEXT("ShaderTech");
	}
};