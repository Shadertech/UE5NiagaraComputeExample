// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/GlobalLib.h"

#if WITH_EDITOR
#include "DrawDebugHelpers.h"
#include "Components/BillboardComponent.h"
#endif
#include "Utils/RPCSManager.h"
#include "Utils/ManagedRPCSInterface.h"
#include "RHICommandList.h"
#include "ComputeActorBase.generated.h"

UCLASS(Abstract)
class COMPUTECORE_API AComputeActorBase : public AActor, public IManagedRPCSInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AComputeActorBase();

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

	UFUNCTION(BlueprintPure, Category = "ComputeExample", meta = (BlueprintThreadSafe, WorldContext = "WorldContextObject"))
	static bool IsPlaying(const UObject* WorldContextObject);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Bounds")
	USceneComponent* Root = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Bounds")
	bool bDebugSprite = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Bounds")
	bool bDebugBounds = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "Bounds")
	bool bDebugDisplayInRuntime = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounds")
	FBoundsConstantParameters BoundsConstantParameters;

#if WITH_EDITORONLY_DATA
public:
	virtual bool ShouldTickIfViewportsOnly() const override;
	virtual void SelectedTick();
	virtual void OnSelected();
	virtual void OnUnselected();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Bounds")
	UBillboardComponent* Billboard = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounds")
	FColor BoundsColor = FColor::Yellow;
	UPROPERTY(Transient)
	FColor CurrentBoundsColor = FColor::Black;
	bool bPreviouslySelected = false;
#endif

protected:
	float LastDeltaTime = 0.0f;

	UPROPERTY(Transient)
	URPCSManager* cachedRPCSManager = nullptr;

	virtual FString GetOwnerName() const;
};

inline FString AComputeActorBase::GetOwnerName() const
{
	return "";
}