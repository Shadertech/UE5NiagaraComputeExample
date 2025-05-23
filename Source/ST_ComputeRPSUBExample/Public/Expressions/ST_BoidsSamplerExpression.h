// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

/**
 * Absolute value material expression for user-defined materials
 *
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "MaterialExpressionIO.h"
#include "Materials/MaterialExpression.h"
#include "ST_BoidsSamplerExpression.generated.h"

UCLASS(MinimalAPI, collapsecategories, hidecategories=Object)
class UST_BoidsSamplerExpression : public UMaterialExpression
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(meta = (RequiredInput = "true", ToolTip = "The id of the boid that should be sampled."))
	FExpressionInput BufferId;

	//~ Begin UMaterialExpression Interface
#if WITH_EDITOR
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
#endif
	//~ End UMaterialExpression Interface

};