#include "SceneUBtoMatManager.h"
#include "SceneUBtoMatSceneViewExtension.h"

void USceneUBtoMatManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ViewExtension = FSceneViewExtensions::NewExtension<FSceneUBtoMatSceneViewExtension>();
}

void USceneUBtoMatManager::Deinitialize()
{
	Super::Deinitialize();
}
