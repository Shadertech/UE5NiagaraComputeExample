// Copyright Epic Games, Inc. All Rights Reserved.

#include "Expressions/BoidsSamplerExpression.h"

#include "Materials/MaterialExpressionCustom.h"

#include "MaterialCompiler.h"

#define LOCTEXT_NAMESPACE "BoidsSamplerExpression"

const FString PATH_MATERIALEXAMPLELIB = TEXT("/ComputeRPSUBExample/Libs/MaterialExampleLib.ush");

UBoidsSamplerExpression::UBoidsSamplerExpression(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FText NAME_Input;
		// This is used for placing the expression in the correct category
		// You can reference multiple categories here, see: https://github.com/EpicGames/UnrealEngine/blob/ue5-main/Engine/Source/Runtime/Engine/Private/Materials/MaterialExpressions.cpp#L18670
		FConstructorStatics()
			: NAME_Input(LOCTEXT( "BoidsSampler", "BoidsSampler" ))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

#if WITH_EDITORONLY_DATA
	MenuCategories.Add(ConstructorStatics.NAME_Input);
#endif

#if WITH_EDITORONLY_DATA
	bShowOutputNameOnPin = true;

	Outputs.Reset();
	Outputs.Add(FExpressionOutput(TEXT("NumBoids"), 1, 1, 0, 0, 0));
	Outputs.Add(FExpressionOutput(TEXT("SampledPosition"), 1, 1, 1, 1, 0));
	Outputs.Add(FExpressionOutput(TEXT("SampledVelocity"), 1, 1, 1, 1, 0));
	Outputs.Add(FExpressionOutput(TEXT("SampledColour"), 1, 1, 1, 1, 1));
#endif
}

#if WITH_EDITOR
int32 UBoidsSamplerExpression::Compile( FMaterialCompiler* Compiler, int32 OutputIndex)
{
	UMaterialExpressionCustom* CustomExpression = NewObject<UMaterialExpressionCustom>(this);
	CustomExpression->IncludeFilePaths.Add(PATH_MATERIALEXAMPLELIB);
	
	switch (OutputIndex)
	{
		case 0:
		{
			CustomExpression->OutputType = ECustomMaterialOutputType::CMOT_Float1;
			CustomExpression->Code = TEXT("return GetNumBoids();");
			return CustomExpression->Compile(Compiler, 0);
		}
			break;
		case 1:
		{
			CustomExpression->OutputType = ECustomMaterialOutputType::CMOT_Float3;

			FCustomInput CodeInput;
			CodeInput.InputName = "id";
			CodeInput.Input = BufferId;

			FCustomOutput CodeOutput;
			CodeOutput.OutputName = "OutValue";
			CodeOutput.OutputType = ECustomMaterialOutputType::CMOT_Float3;
			CustomExpression->AdditionalOutputs.Add(CodeOutput);

			CustomExpression->Inputs.Add(CodeInput);
			CustomExpression->Code = TEXT("bool IsValid;\nSampleBoidsPosition(id, IsValid, OutValue);\nreturn 1;");
			return CustomExpression->Compile(Compiler, 1);
		}
			break;
		case 2:
		{
			CustomExpression->OutputType = ECustomMaterialOutputType::CMOT_Float3;

			FCustomInput CodeInput;
			CodeInput.InputName = "id";
			CodeInput.Input = BufferId;
			CustomExpression->Inputs.Add(CodeInput);

			CustomExpression->Code = TEXT("bool IsValid;\nfloat3 OutValue;\nSampleBoidsVelocity(id, IsValid, OutValue);\nreturn OutValue;");
			return CustomExpression->Compile(Compiler, 0);
		}
			break;
		case 3:
		{
			CustomExpression->OutputType = ECustomMaterialOutputType::CMOT_Float4;

			FCustomInput CodeInput;
			CodeInput.InputName = "id";
			CodeInput.Input = BufferId;

			FCustomOutput CodeOutput;
			CodeOutput.OutputName = "OutValue";
			CodeOutput.OutputType = ECustomMaterialOutputType::CMOT_Float4;
			CustomExpression->AdditionalOutputs.Add(CodeOutput);

			CustomExpression->Inputs.Add(CodeInput);
			CustomExpression->Code = TEXT("bool IsValid;\nSampleBoidsColour(id, IsValid, OutValue);\nreturn 1;");
			return CustomExpression->Compile(Compiler, 1);
		}
			break;
		default:
			break;
	}

	return Compiler->Errorf(TEXT("Invalid input parameter"));
}

void UBoidsSamplerExpression::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(TEXT("Sample Boids"));
}
#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE