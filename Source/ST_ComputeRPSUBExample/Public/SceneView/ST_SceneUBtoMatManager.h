// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once
#include "Subsystems/EngineSubsystem.h"
#include "ST_SceneUBtoMatManager.generated.h"

UCLASS()
class UST_SceneUBtoMatManager : public UEngineSubsystem
{
	GENERATED_BODY()
public:
	//Begin UEngineSubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//End UEngineSubsystem Interface

	TSharedPtr<class FST_SceneUBtoMatSceneViewExtension> GetSceneViewExtension() const;
protected:
	TSharedPtr<class FST_SceneUBtoMatSceneViewExtension, ESPMode::ThreadSafe> ViewExtension;
};

inline TSharedPtr<class FST_SceneUBtoMatSceneViewExtension> UST_SceneUBtoMatManager::GetSceneViewExtension() const
{
	return ViewExtension;
}