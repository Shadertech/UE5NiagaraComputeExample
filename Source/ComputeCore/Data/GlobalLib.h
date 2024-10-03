// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GlobalLib.generated.h"

USTRUCT(BlueprintType)
struct COMPUTECORE_API FBoundsConstantParameters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Bounds Constants")
	bool bSphere = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounds Constants", meta = (EditCondition = "bSphere"))
	float Radius = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounds Constants", meta = (EditCondition = "!bSphere"))
	FVector BoundsExtent = FVector(100, 100, 100);
};