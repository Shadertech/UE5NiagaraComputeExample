#pragma once
#include "Subsystems/EngineSubsystem.h"
#include "SceneUBtoMatManager.generated.h"

UCLASS()
class USceneUBtoMatManager : public UEngineSubsystem
{
	GENERATED_BODY()
public:
	//Begin UEngineSubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//End UEngineSubsystem Interface

	TSharedPtr<class FSceneUBtoMatSceneViewExtension> GetSceneViewExtension() const;
protected:
	TSharedPtr<class FSceneUBtoMatSceneViewExtension, ESPMode::ThreadSafe> ViewExtension;
};

inline TSharedPtr<class FSceneUBtoMatSceneViewExtension> USceneUBtoMatManager::GetSceneViewExtension() const
{
	return ViewExtension;
}