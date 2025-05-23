// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "NiagaraSystem.h"
#include "Data/ST_BoidsLib.h"
#include "ST_ComputeRPLegacyExampleSettings.generated.h"

UCLASS(Config = ST_NiagaraCSIntegration, DefaultConfig, meta = (DisplayName = "ST Compute RP Legacy Example Settings"))
class ST_COMPUTERPLEGACYEXAMPLE_API UST_ComputeRPLegacyExampleSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "ShaderTech|ComputeRPLegacyExampleSettings")
	static const UST_ComputeRPLegacyExampleSettings* GetComputeRPLegacyExampleSettings()
	{
		return GetDefault<UST_ComputeRPLegacyExampleSettings>();
	}

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Boids")
	TSoftObjectPtr<UNiagaraSystem> BoidsEmitterLegacyVFX = nullptr;

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