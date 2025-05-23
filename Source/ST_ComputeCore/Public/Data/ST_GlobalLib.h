// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ST_GlobalLib.generated.h"

USTRUCT(BlueprintType)
struct ST_COMPUTECORE_API FST_BoundsConstantParameters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "ShaderTech|BoundsConstants")
	bool bSphere = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShaderTech|BoundsConstants", meta = (EditCondition = "bSphere"))
	float Radius = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShaderTech|BoundsConstants", meta = (EditCondition = "!bSphere"))
	FVector BoundsExtent = FVector(100, 100, 100);
};