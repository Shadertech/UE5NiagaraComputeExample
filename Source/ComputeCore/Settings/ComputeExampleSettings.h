#pragma once

#include "Engine/DeveloperSettings.h"
#include "NiagaraSystem.h"
#include "Data/BoidsLib.h"
#include "ComputeExampleSettings.generated.h"

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Compute Example Settings"))
class COMPUTECORE_API UComputeExampleSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "ComputeExample")
	static const UComputeExampleSettings* GetComputeExampleSettings()
	{
		return GetDefault<UComputeExampleSettings>();
	}

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Boids")
	TSoftObjectPtr<UNiagaraSystem> BoidsEmitterVFX = nullptr;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Boids")
	TSoftObjectPtr<UNiagaraSystem> BoidsEmitterMaterialVFX = nullptr;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Boids")
	TSoftObjectPtr<UNiagaraSystem> BoidsEmitterLegacyVFX = nullptr;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Boids")
	TSoftObjectPtr<UNiagaraSystem> BoidsDrawerVFX = nullptr;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Boids")
	TSoftObjectPtr<UNiagaraSystem> BoidsVFX = nullptr;

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Boids")
	FBoidConstantParameters BoidConstantParameters;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Boids")
	FBoidDynamicParameters BoidDynamicParameters;

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Material")
	TSoftObjectPtr<UMaterialInterface> BoidsMatInterface = nullptr;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = "Material")
	TSoftObjectPtr<UMaterialInterface> BoidsDrawMatInterface = nullptr;
};