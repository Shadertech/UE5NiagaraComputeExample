// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

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

	if (ViewExtension != nullptr)
	{
		ViewExtension.Reset();
		ViewExtension = nullptr;
	}
}
