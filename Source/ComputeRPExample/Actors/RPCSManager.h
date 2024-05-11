#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/ManagedRPCSInterface.h"
#include "RPCSManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPCSManager, Log, All);

UCLASS()
class COMPUTERPEXAMPLE_API ARPCSManager : public AActor
{
	GENERATED_BODY()
	
public:
	ARPCSManager();

	TArray<IManagedRPCSInterface*> ManagedCSs;

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Compute Shader Manager")
	void DispatchComputeShaders(float DeltaTime=0.016667);

	void Register(IManagedRPCSInterface* ManagedCS);
	void Deregister(IManagedRPCSInterface* ManagedCS);

	static ARPCSManager* Get(UWorld* World);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compute Shader Manager")
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compute Shader Manager")
	int32 FrameDelay = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compute Shader Manager")
	int32 MaxFPS = 60;

protected:
	bool bIsInit = false;
	bool bIsExecuting = false;

	FRenderCommandFence RenderCommandFence; // Necessary for waiting until a render command function finishes.
};
