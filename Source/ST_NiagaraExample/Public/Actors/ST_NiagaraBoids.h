// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/ST_BoidsLib.h"
#include "NiagaraComponent.h"
#include "Base/ST_ComputeActorBase.h"

#include "ST_NiagaraBoids.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNiagaraBoids, Log, All);

UCLASS()
class ST_NIAGARAEXAMPLE_API AST_NiagaraBoids : public AST_ComputeActorBase
{
	GENERATED_BODY()

public:
	AST_NiagaraBoids();

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION(BlueprintCallable, Category = "ShaderTech|Niagara")
	bool SetConstantParameters();
	UFUNCTION(BlueprintCallable, Category = "ShaderTech|Niagara")
	bool SetDynamicParameters();

protected:
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "ShaderTech|BoidParameters")
	FST_BoidCurrentParameters BoidCurrentParameters;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Niagara")
	UNiagaraComponent* Niagara = nullptr;

private:
	FMatrix BoundsMatrix;
	UPROPERTY(Transient)
	bool ConstantsSet = false;
};