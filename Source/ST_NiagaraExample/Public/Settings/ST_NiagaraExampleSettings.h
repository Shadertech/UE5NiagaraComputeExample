// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "NiagaraSystem.h"
#include "Data/ST_BoidsLib.h"
#include "ST_NiagaraExampleSettings.generated.h"

UCLASS(Config = ST_NiagaraCSIntegration, DefaultConfig, meta = (DisplayName = "ST Niagara Example Settings"))
class ST_NIAGARAEXAMPLE_API UST_NiagaraExampleSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "ShaderTech|NiagaraExampleSettings")
	static const UST_NiagaraExampleSettings* GetNiagaraExampleSettings()
	{
		return GetDefault<UST_NiagaraExampleSettings>();
	}

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Boids")
	TSoftObjectPtr<UNiagaraSystem> BoidsVFX = nullptr;

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