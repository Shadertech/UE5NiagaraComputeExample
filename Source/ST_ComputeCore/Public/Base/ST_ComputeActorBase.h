// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/ST_GlobalLib.h"

#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#include "Components/BillboardComponent.h"
#endif
#include "Utils/ST_RPCSManager.h"
#include "Utils/ST_ManagedRPCSInterface.h"
#include "RHICommandList.h"
#include "ST_ComputeActorBase.generated.h"

UCLASS(Abstract)
class ST_COMPUTECORE_API AST_ComputeActorBase : public AActor, public IST_ManagedRPCSInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AST_ComputeActorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	// Start Managed Compute Shader Interface 
	virtual void InitComputeShader_GameThread() override;
	virtual void InitComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList) override;

	virtual void ExecuteComputeShader_GameThread(float DeltaTime) override;
	virtual void ExecuteComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList) override;

	virtual void DisposeComputeShader_GameThread() override;
	virtual void DisposeComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList) override;
	// End Managed Compute Shader Interface 

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "ShaderTech|ComputeExample", meta = (BlueprintThreadSafe, WorldContext = "WorldContextObject"))
	static bool IsPlaying(const UObject* WorldContextObject);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "ShaderTech|Bounds")
	USceneComponent* Root = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "ShaderTech|Bounds")
	bool bDebugSprite = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "ShaderTech|Bounds")
	bool bDebugBounds = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "ShaderTech|Bounds")
	bool bDebugDisplayInRuntime = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShaderTech|Bounds")
	FST_BoundsConstantParameters BoundsConstantParameters;

#if WITH_EDITORONLY_DATA
public:
	virtual bool ShouldTickIfViewportsOnly() const override;
	virtual void SelectedTick();
	virtual void OnSelected();
	virtual void OnUnselected();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "ShaderTech|Bounds")
	UBillboardComponent* Billboard = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShaderTech|Bounds")
	FColor BoundsColor = FColor::Yellow;
	UPROPERTY(Transient)
	FColor CurrentBoundsColor = FColor::Black;
	bool bPreviouslySelected = false;
#endif

protected:
	float LastDeltaTime = 0.0f;

	UPROPERTY(Transient)
	UST_RPCSManager* cachedRPCSManager = nullptr;

	virtual FString GetOwnerName() const;
};

inline FString AST_ComputeActorBase::GetOwnerName() const
{
	return "";
}