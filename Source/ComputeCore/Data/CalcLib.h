// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "RHIGPUReadback.h"
#include "CalcLib.generated.h"

USTRUCT(BlueprintType)
struct COMPUTECORE_API FCalcItem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CalcItem")
	float it = 0.0f;
};

struct FCalcGPUReadback
{
	FRHIGPUBufferReadback Readback;
	int32 NrWorkGroups;
	int32 BufferLength;
	TArray<FCalcItem> Result;
	bool bReadbackComplete;

	FCalcGPUReadback()
		: Readback(TEXT("CalcReadbackTest"))
		, NrWorkGroups(0)
		, BufferLength(0)
		, Result()
		, bReadbackComplete(false)
	{
	}
};

struct FCalcGPUDispatches
{
	TMap<int32, FCalcGPUReadback> CalcGPURequests;
	FCriticalSection DispatchLock;
	int32 RequestId = 0;
};