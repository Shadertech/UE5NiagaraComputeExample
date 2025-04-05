// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Utils/ManagedRPCSInterface.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "RPCSManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPCSManager, Log, All);

UCLASS()
class COMPUTECORE_API URPCSManager : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	URPCSManager();

	TArray<IManagedRPCSInterface*> ManagedCSs;

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

	UFUNCTION(BlueprintCallable, Category = "Compute Shader Manager")
	void DispatchComputeShaders(float DeltaTime=0.016667);

	void Register(IManagedRPCSInterface* ManagedCS);
	void Deregister(IManagedRPCSInterface* ManagedCS);

	static URPCSManager* Get(UWorld* World);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compute Shader Manager")
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compute Shader Manager")
	int32 MaxFPS = 60;

protected:
	bool bIsExecuting = false;

	FRenderCommandFence RenderCommandFence; // Necessary for waiting until a render command function finishes.
};
