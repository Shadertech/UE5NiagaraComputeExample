// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/ComputeActorBase.h"

// Sets default values
AComputeActorBase::AComputeActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

#if WITH_EDITOR
	// Billboard
	Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	Billboard->SetHiddenInGame(false, true);
	Billboard->SetupAttachment(RootComponent);
#endif // WITH_EDITOR
}

// Called when the game starts or when spawned
void AComputeActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called when the game ends or when destroyed
void AComputeActorBase::BeginDestroy()
{
	Super::BeginDestroy();

}

// Called every frame
void AComputeActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if WITH_EDITOR
	//UE_LOG(LogBoids, Log, TEXT("bDebug %d, IsSelected() %d"), bDebug ? 1 : 0, IsSelected()? 1 : 0);

	if (bDebugSprite)
	{
		Billboard->SetHiddenInGame(false, true);
		Billboard->SetVisibility(true);
	}
	else {
		Billboard->SetHiddenInGame(true, true);
		Billboard->SetVisibility(false);
	}

	if (bDebugBounds || IsSelected())
	{
		
		//UE_LOG(LogBoids, Log, TEXT("Draw"));

		FColor Color = BoundsColor;

		if (!IsSelected())
		{
			Color.R = FMath::RoundToInt(Color.R * 0.5f);
			Color.G = FMath::RoundToInt(Color.G * 0.5f);
			Color.B = FMath::RoundToInt(Color.B * 0.5f);
		}

		if (BoundsConstantParameters.bSphere)
		{
			int32 Segments = 8;
			DrawDebugSphere(GetWorld(), GetActorLocation(), BoundsConstantParameters.Radius, Segments, Color);
		}
		else
		{
			DrawDebugBox(GetWorld(), GetActorLocation(), BoundsConstantParameters.BoundsExtent, Color);
		}
	}

#endif // WITH_EDITOR
}

#if WITH_EDITOR
bool AComputeActorBase::ShouldTickIfViewportsOnly() const
{
	// Tick in Editor
	return true;
}
#endif