// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RHIGPUReadback.h"
#include "RenderGraphDefinitions.h"
#include "BoidsLib.generated.h"

/// <summary>
/// PADDING IS IMPORTANT FOR SCENE VIEW
/// </summary>

USTRUCT(BlueprintType)
struct COMPUTECORE_API FBoidItem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BoidItem")
	FVector3f pos = FVector3f(0.0f, 0.0f, 0.0f);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BoidItem")
	FVector3f vel = FVector3f(0.0f, 0.0f, 0.0f);
	int32 padd0 = 0;
	int32 padd1 = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BoidItem")
	FVector4f col = FVector4f(0.0f, 0.0f, 0.0f, 0.0f);
};

//struct FBoidItem
//{
//	float pos[3];
//	float vel[3];
//	float col[3];
//};

struct FBoidsGPUReadback
{
	FRHIGPUBufferReadback Readback;
	int32 NrWorkGroups;
	int32 BufferLength;
	TArray<FBoidItem> Result;
	bool bReadbackComplete;

	FBoidsGPUReadback()
		: Readback(TEXT("BoidsReadbackTest"))
		, NrWorkGroups(0)
		, BufferLength(0)
		, Result()
		, bReadbackComplete(false)
	{
	}
};

struct FBoidsGPUDispatches
{
	TMap<int32, FBoidsGPUReadback> BoidsGPURequests;
	FCriticalSection DispatchLock;
	int32 RequestId = 0;
};

struct FBoidsRDGStateData
{
	TArray<FRDGPassRef> InitPass;
	TArray<FRDGPassRef> ExecutePass;

	FGPUFenceRHIRef InitFence;

	FBoidsRDGStateData()
	{
		InitPass.SetNum(0);
		ExecutePass.SetNum(0);
		InitFence = nullptr;
	}

	FBoidsRDGStateData(int32 InitPassSize, int32 ExecutePassSize)
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
struct COMPUTECORE_API FBoidConstantParameters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Constants")
	int32 numBoids = 32;
};

USTRUCT(BlueprintType)
struct COMPUTECORE_API FBoidDynamicParameters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Variables")
	float simulationSpeed = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Variables")
	float maxSpeed = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Variables")
	float minDistance = 0.15f;
	float minDistanceSq() const { return minDistance * minDistance; }
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Variables")
	float cohesionFactor = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Variables")
	float separationFactor = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Variables")
	float alignmentFactor = 5.0f;
	float turnSpeed() const { return maxSpeed * 3.0f; }
	float minSpeed() const { return maxSpeed * 0.75f; }
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Variables")
	float meshScale = 0.02f;
};

USTRUCT(BlueprintType)
struct COMPUTECORE_API FBoidCurrentParameters
{
	GENERATED_BODY()

public:
	FBoidConstantParameters ConstantParameters;
	FBoidDynamicParameters DynamicParameters;
	float worldScale = 1.0f;
	float boundsRadius;
	FMatrix44f transformMatrix;
	FMatrix44f inverseTransformMatrix;
};