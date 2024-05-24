#pragma once

#include "CoreMinimal.h"
#include "Data/BoidsLib.h"
#include "NiagaraComponent.h"
#include "Base/ComputeActorBase.h"

#include "NiagaraBoids.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNiagaraBoids, Log, All);

UCLASS()
class NIAGARAEXAMPLE_API ANiagaraBoids : public AComputeActorBase
{
	GENERATED_BODY()

public:
	ANiagaraBoids();

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION(BlueprintCallable, Category = "Niagara")
	bool SetConstantParameters();
	UFUNCTION(BlueprintCallable, Category = "Niagara")
	bool SetDynamicParameters();

protected:
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Boid Parameters")
	FBoidCurrentParameters BoidCurrentParameters;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Niagara")
	UNiagaraComponent* Niagara = nullptr;

private:
	FMatrix BoundsMatrix;
	UPROPERTY(Transient)
	bool ConstantsSet = false;
};