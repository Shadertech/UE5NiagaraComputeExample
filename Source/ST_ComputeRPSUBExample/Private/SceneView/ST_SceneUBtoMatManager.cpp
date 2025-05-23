// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "SceneView/ST_SceneUBtoMatManager.h"
#include "SceneView/ST_SceneUBtoMatSceneViewExtension.h"

void UST_SceneUBtoMatManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ViewExtension = FSceneViewExtensions::NewExtension<FST_SceneUBtoMatSceneViewExtension>();
}

void UST_SceneUBtoMatManager::Deinitialize()
{
	Super::Deinitialize();

	if (ViewExtension != nullptr)
	{
		ViewExtension.Reset();
		ViewExtension = nullptr;
	}
}
