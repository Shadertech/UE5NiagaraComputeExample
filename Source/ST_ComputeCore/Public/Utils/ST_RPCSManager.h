// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Utils/ST_ManagedRPCSInterface.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "RenderCommandFence.h"
#include "ST_RPCSManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPCSManager, Log, All);

UCLASS()
class ST_COMPUTECORE_API UST_RPCSManager : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	UST_RPCSManager();

	TArray<IST_ManagedRPCSInterface*> ManagedCSs;

protected:
	// Initialization function, called when the subsystem is created
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Function called before the subsystem is destroyed
	virtual void Deinitialize() override;

public:

	// FTickableGameObject overrides
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;

	UFUNCTION(BlueprintCallable, Category = "ShaderTech|ComputeShaderManager")
	void DispatchComputeShaders(float DeltaTime=0.016667);

	void Register(IST_ManagedRPCSInterface* ManagedCS);
	void Deregister(IST_ManagedRPCSInterface* ManagedCS);

	static UST_RPCSManager* Get(UWorld* World);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShaderTech|ComputeShaderManager")
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShaderTech|ComputeShaderManager")
	int32 MaxFPS = 60;

protected:
	bool bIsExecuting = false;

	FRenderCommandFence RenderCommandFence; // Necessary for waiting until a render command function finishes.
};
