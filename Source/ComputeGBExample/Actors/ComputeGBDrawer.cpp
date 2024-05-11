#include "ComputeGBDrawer.h"
#include "Kismet/GameplayStatics.h"
#include "SceneRendering.h"
#include "ScenePrivate.h"
#include "ComputeGBExample.h"
#include "ComputeShaders/BoidsCS.h"
#include "ComputeShaders/BoidsDrawerCS.h"
#include "PixelShaders/BoidsDrawerPS.h"
#include "Engine/World.h"
#include "NiagaraUserRedirectionParameterStore.h"
#include "NiagaraDataInterfaceTexture.h"

DEFINE_LOG_CATEGORY(LogComputeGBDrawer);

// ____________________________________________ Constructor

AComputeGBDrawer::AComputeGBDrawer()
{
	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(RootComponent);
	Niagara->bAutoActivate = false;

	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	Plane->SetupAttachment(RootComponent);
}

void AComputeGBDrawer::BeginPlay()
{
	CreateRenderTexture();

	if (material != nullptr && RenderTarget != nullptr)
	{
		UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(material, nullptr);
		Plane->SetMaterial(0, MID);
		MID->SetTextureParameterValue(FName("BoidsPos"), RenderTarget);
	}

	Super::BeginPlay();

	FComputeGBExampleModule::Get().Register(this);
}

void AComputeGBDrawer::BeginDestroy()
{
	if (Niagara != nullptr )
	{
		Niagara->DeactivateImmediate();
	}

	FComputeGBExampleModule::Get().Deregister(this);

	Super::BeginDestroy();

	if (RenderTarget)
	{
		RenderTarget->ConditionalBeginDestroy(); // Destroy the render target texture
		RenderTarget = nullptr; // Reset the pointer
	}
}

// ____________________________________________ Tick
void AComputeGBDrawer::Tick(float DeltaTime)
{
	LastDeltaTime = DeltaTime;

	SetNiagaraVariableParameters();

	Super::Tick(DeltaTime);
}

bool AComputeGBDrawer::SetNiagaraConstantParameters()
{
	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return false;
	}

	// Get the parameter store of the Niagara component
	FNiagaraUserRedirectionParameterStore& ParameterStore = Niagara->GetOverrideParameters();

	FNiagaraVariable TextureParameter = FNiagaraVariable(FNiagaraTypeDefinition(UNiagaraDataInterfaceTexture::StaticClass()), TEXT("boidPositions"));
	UNiagaraDataInterfaceTexture* data = (UNiagaraDataInterfaceTexture*)ParameterStore.GetDataInterface(TextureParameter);

	if (data)
	{
		data->Texture = RenderTarget;
	}

	Niagara->SetIntParameter("numBoids", BoidConstantParameters.numBoids);

	return true;
}

void AComputeGBDrawer::SetNiagaraVariableParameters()
{
	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return;
	}

	Niagara->SetFloatParameter("meshScale", BoidVariableParameters.meshScale);
	Niagara->SetFloatParameter("worldScale", BoidVariableParameters.worldScale);
}

void AComputeGBDrawer::CreateRenderTexture()
{
	if (RenderTarget != nullptr)
	{
		return;
	}

	FName TextureName = FName(TEXT("RT_BoidPositions"));
	int32 SizeX = 32; // Width
	int32 SizeY = 32; // Height

	RenderTarget = NewObject<UTextureRenderTarget2D>(this, TextureName);
	RenderTarget->ClearColor = FLinearColor::Black;
	RenderTarget->TargetGamma = 0.0f;
	RenderTarget->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA32f;
	RenderTarget->AddressX = TextureAddress::TA_Wrap;
	RenderTarget->AddressY = TextureAddress::TA_Wrap;
	RenderTarget->InitAutoFormat(SizeX, SizeY);
	RenderTarget->UpdateResourceImmediate(true);
}

void AComputeGBDrawer::InitComputeShader()
{
	check(IsInGameThread());

	BoundsMatrix = FMatrix::Identity.ConcatTranslation(GetActorLocation());
	BoidsArray.Empty(); // Clear any existing elements in the array

	// Resize the array to accommodate numboids elements
	BoidsArray.SetNum(BoidConstantParameters.numBoids);

	BoidsPingPongBuffer = FPingPongBuffer();
	BoidsRenderGraphPasses = FBoidsRenderGraphPasses();

	if (SetNiagaraConstantParameters())
	{
		Niagara->Activate(true);
	}
}

void AComputeGBDrawer::ExecuteComputeShader_RenderThread(FRDGBuilder& GraphBuilder)
{
	check(IsInRenderingThread());

	BoidsRenderGraphPasses.ClearPasses();

	BoidVariableParameters.boundsRadius = BoundsConstantParameters.Radius;
	BoidVariableParameters.transformMatrix = (FMatrix44f)BoundsMatrix;
	BoidVariableParameters.inverseTransformMatrix = (FMatrix44f)BoundsMatrix.Inverse();

	if (!BoidsRenderGraphPasses.init)
	{
		FComputeShader_Boids::InitBoidsExample_RenderThread(GraphBuilder, TEXT("ComputeGBDrawer_"), BoidsArray, BoidConstantParameters, BoidVariableParameters, BoidsRenderGraphPasses, BoidsPingPongBuffer);
		FComputeShader_BoidsDrawer::InitBoidsDrawerExample_RenderThread(GraphBuilder, BoidsArray, BoidsPingPongBuffer);
		FPixelShader_BoidsDrawer::InitDrawToRenderTargetExample_RenderThread(GraphBuilder, BoidsArray, RenderTarget, RenderTargetTexture);
	}

	// The graph will help us figure out when the GPU memory is needed, and only have it allocated from then, so this makes memory management a lot easier and nicer!
	//FRDGTextureDesc ComputeShaderOutputDesc = FRDGTextureDesc::Create2D(FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY), EPixelFormat::PF_R32_UINT, FClearValueBinding::None, ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::UAV | ETextureCreateFlags::ShaderResource);
	FRDGTextureDesc ComputeShaderOutputDesc = FRDGTextureDesc::Create2D(FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY), EPixelFormat::PF_A32B32G32R32F, FClearValueBinding::None, ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::UAV | ETextureCreateFlags::ShaderResource);
	FRDGTextureRef OutputTexture = GraphBuilder.CreateTexture(ComputeShaderOutputDesc, TEXT("ShaderPlugin_ComputeShaderOutput"), ERDGTextureFlags::None);
	FComputeShader_Boids::DispatchBoidsExample_RenderThread(GraphBuilder, TEXT("ComputeGBDrawer_"), BoidConstantParameters, BoidVariableParameters, BoidsRenderGraphPasses, BoidsPingPongBuffer, LastDeltaTime);
	FComputeShader_BoidsDrawer::DispatchBoidsDrawerExample_RenderThread(GraphBuilder, BoidConstantParameters, BoidsPingPongBuffer, GraphBuilder.CreateUAV(OutputTexture), RenderTarget);
	FPixelShader_BoidsDrawer::DrawToRenderTargetExample_RenderThread(GraphBuilder, BoidConstantParameters, GraphBuilder.CreateSRV(OutputTexture), RenderTarget, RenderTargetTexture);
}

void AComputeGBDrawer::DisposeComputeShader()
{
	check(IsInGameThread());
	RenderTargetTexture = nullptr;
}