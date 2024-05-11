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
	bool SetNiagaraConstantParameters();
	UFUNCTION(BlueprintCallable, Category = "Niagara")
	void SetNiagaraVariableParameters();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Parameters")
	FBoidConstantParameters BoidConstantParameters;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Parameters")
	FBoidVariableParameters BoidVariableParameters;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Niagara")
	UNiagaraComponent* Niagara = nullptr;

private:
	FMatrix BoundsMatrix;
	UPROPERTY(Transient)
	bool ConstantsSet = false;
};