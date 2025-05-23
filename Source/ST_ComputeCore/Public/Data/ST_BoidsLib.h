// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RHIGPUReadback.h"
#include "RenderGraphDefinitions.h"
#include "ST_BoidsLib.generated.h"

/// <summary>
/// PADDING IS IMPORTANT FOR SCENE VIEW
/// </summary>

USTRUCT(BlueprintType)
struct ST_COMPUTECORE_API FST_BoidItem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShaderTech|BoidItem")
	FVector3f pos = FVector3f(0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShaderTech|BoidItem")
	FVector3f vel = FVector3f(0.0f, 0.0f, 0.0f);
	int32 padd0 = 0;
	int32 padd1 = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShaderTech|BoidItem")
	FVector4f col = FVector4f(0.0f, 0.0f, 0.0f, 0.0f);
};

//struct FST_BoidItem
//{
//	float pos[3];
//	float vel[3];
//	float col[3];
//};

struct FST_BoidsGPUReadback
{
	FRHIGPUBufferReadback Readback;
	int32 NrWorkGroups;
	int32 BufferLength;
	TArray<FST_BoidItem> Result;
	bool bReadbackComplete;

	FST_BoidsGPUReadback()
		: Readback(TEXT("BoidsReadbackTest"))
		, NrWorkGroups(0)
		, BufferLength(0)
		, Result()
		, bReadbackComplete(false)
	{
	}
};

struct FST_BoidsGPUDispatches
{
	TMap<int32, FST_BoidsGPUReadback> BoidsGPURequests;
	FCriticalSection DispatchLock;
	int32 RequestId = 0;
};

struct FST_BoidsRDGStateData
{
	TArray<FRDGPassRef> InitPass;
	TArray<FRDGPassRef> ExecutePass;

	FGPUFenceRHIRef InitFence;

	FST_BoidsRDGStateData()
	{
		InitPass.SetNum(0);
		ExecutePass.SetNum(0);
		InitFence = nullptr;
	}

	FST_BoidsRDGStateData(int32 InitPassSize, int32 ExecutePassSize)
	{
		InitPass.SetNum(InitPassSize);
		ExecutePass.SetNum(ExecutePassSize);

		InitFence = nullptr;
	};

	void ClearPasses()
	{
		for (int32 Index = 0; Index < InitPass.Num(); ++Index)
		{
			InitPass[Index] = nullptr;
		}
		for (int32 Index = 0; Index < ExecutePass.Num(); ++Index)
		{
			ExecutePass[Index] = nullptr;
		}
	}

	void Dispose()
	{
		if (InitFence->IsValid())
		{
			InitFence->Clear();
			InitFence = nullptr;
		}
	}

	bool WaitingOnInitFence()
	{
		return (!InitFence.IsValid() || !InitFence->Poll());
	}
};

USTRUCT(BlueprintType)
struct ST_COMPUTECORE_API FST_BoidConstantParameters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShaderTech|BoidConstants")
	int32 numBoids = 32;
};

USTRUCT(BlueprintType)
struct ST_COMPUTECORE_API FST_BoidDynamicParameters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShaderTech|BoidVariables")
	float simulationSpeed = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShaderTech|BoidVariables")
	float maxSpeed = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShaderTech|BoidVariables")
	float minDistance = 0.15f;
	float minDistanceSq() const { return minDistance * minDistance; }
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShaderTech|BoidVariables")
	float cohesionFactor = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShaderTech|BoidVariables")
	float separationFactor = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShaderTech|BoidVariables")
	float alignmentFactor = 5.0f;
	float turnSpeed() const { return maxSpeed * 3.0f; }
	float minSpeed() const { return maxSpeed * 0.75f; }
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShaderTech|BoidVariables")
	float meshScale = 0.02f;
};

USTRUCT(BlueprintType)
struct ST_COMPUTECORE_API FST_BoidCurrentParameters
{
	GENERATED_BODY()

public:
	FST_BoidConstantParameters ConstantParameters;
	FST_BoidDynamicParameters DynamicParameters;
	float worldScale = 1.0f;
	float boundsRadius;
	FMatrix44f transformMatrix;
	FMatrix44f inverseTransformMatrix;
};