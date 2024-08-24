// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/CSGlobalLib.h"

#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#include "Components/BillboardComponent.h"
#endif

#include "ComputeActorBase.generated.h"

UCLASS()
class COMPUTECORE_API AComputeActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AComputeActorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Bounds")
	USceneComponent* Root = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Bounds")
	bool bDebugSprite = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Bounds")
	bool bDebugBounds = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounds")
	FBoundsConstantParameters BoundsConstantParameters;

#if WITH_EDITORONLY_DATA
public:
	virtual bool ShouldTickIfViewportsOnly() const override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Bounds")
	UBillboardComponent* Billboard = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounds")
	FColor BoundsColor = FColor::Yellow;
#endif
};
