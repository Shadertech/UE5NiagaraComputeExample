#include "ComputeRPEmitter.h"
#include "Kismet/GameplayStatics.h"
#include "SceneRendering.h"
#include "ScenePrivate.h"
#include "Niagara/NiagaraDataInterfaceStructuredBuffer.h"
#include "ComputeShaders/BoidsRPCS.h"

#define BoidsExample_ThreadsPerGroup 512
DEFINE_LOG_CATEGORY(LogComputeRPEmitter);

// ____________________________________________ Constructor

AComputeRPEmitter::AComputeRPEmitter()
{
	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(RootComponent);
	Niagara->bAutoActivate = false;
}

// ____________________________________________ BeginPlay

void AComputeRPEmitter::BeginPlay()
{
	Super::BeginPlay();

	cachedRPCSManager = ARPCSManager::Get(GetWorld());

	if (cachedRPCSManager != nullptr)
	{
		cachedRPCSManager->Register(this);
	}
}

// ____________________________________________ BeginDestroy

void AComputeRPEmitter::BeginDestroy()
{
	if (Niagara != nullptr)
	{
		Niagara->DeactivateImmediate();
	}
	Super::BeginDestroy();
	if (cachedRPCSManager != nullptr)
	{
		cachedRPCSManager->Deregister(this);
	}
}

// ____________________________________________ Init Compute Shader

void AComputeRPEmitter::InitComputeShader()
{
	check(IsInGameThread());

	BoidVariableParameters.boundsRadius = BoundsConstantParameters.Radius;
	BoidVariableParameters.transformMatrix = (FMatrix44f)BoundsMatrix;
	BoidVariableParameters.inverseTransformMatrix = (FMatrix44f)BoundsMatrix.Inverse();

	BoundsMatrix = FMatrix::Identity.ConcatTranslation(GetActorLocation());
	BoidsArray.Empty(); // Clear any existing elements in the array

	// Resize the array to accommodate numboids elements
	BoidsArray.SetNum(BoidConstantParameters.numBoids);

	BoidsPingPongBuffer = FPingPongBuffer();

	if (SetNiagaraConstantParameters())
	{
		Niagara->Activate(true);
	}
}

void AComputeRPEmitter::InitComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	FRDGBuilder GraphBuilder(RHICmdList);

	FString OwnerName = "ComputeRPEmitter";

	const TCHAR* ReadBufferName = *(FString(OwnerName) + TEXT("BoidsInBuffer"));
	const TCHAR* WriteBufferName = *(FString(OwnerName) + TEXT("BoidsOutBuffer"));
	const TCHAR* SRVName = *(FString(OwnerName) + TEXT("BoidsIn_StructuredBuffer"));
	const TCHAR* UAVName = *(FString(OwnerName) + TEXT("BoidsOut_StructuredBuffer"));

	// First we create an RDG buffer with the appropriate size, and then instruct the graph to upload our CPU data to it.
	FRDGBufferDesc ReadDesc = FRDGBufferDesc::CreateStructuredDesc(BoidsArray.GetTypeSize(), BoidsArray.Num());
	FRDGBufferRef ReadBuffer = GraphBuilder.CreateBuffer(ReadDesc, ReadBufferName);
	GraphBuilder.QueueBufferUpload(ReadBuffer, BoidsArray.GetData(), BoidsArray.GetTypeSize() * BoidsArray.Num(), ERDGInitialDataFlags::None);

	FRDGBufferDesc WriteDesc = FRDGBufferDesc::CreateStructuredDesc(BoidsArray.GetTypeSize(), BoidsArray.Num());
	FRDGBufferRef WriteBuffer = GraphBuilder.CreateBuffer(WriteDesc, WriteBufferName);
	GraphBuilder.QueueBufferUpload(WriteBuffer, BoidsArray.GetData(), BoidsArray.GetTypeSize() * BoidsArray.Num(), ERDGInitialDataFlags::None);

	BoidsPingPongBuffer.ReadPooled = GraphBuilder.ConvertToExternalBuffer(ReadBuffer);
	BoidsPingPongBuffer.WritePooled = GraphBuilder.ConvertToExternalBuffer(WriteBuffer);

	BoidsPingPongBuffer.RegisterRW(GraphBuilder, SRVName, UAVName);

	QUICK_SCOPE_CYCLE_COUNTER(STAT_ComputeRPExample_Init); // Used to gather CPU profiling data for Unreal Insights.
	SCOPED_DRAW_EVENT(RHICmdList, ComputeRPExample_Init); // Used to profile GPU activity and add metadata to be consumed by for example RenderDoc

	FBoidsRPInitExampleCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FBoidsRPInitExampleCS::FParameters>();
	PassParameters->numBoids = BoidConstantParameters.numBoids;
	PassParameters->maxSpeed = BoidVariableParameters.maxSpeed;
	PassParameters->boidsOut = BoidsPingPongBuffer.WriteScopedUAV;

	PassParameters->boundsInverseMatrix = BoidVariableParameters.inverseTransformMatrix;
	PassParameters->boundsMatrix = BoidVariableParameters.transformMatrix;
	PassParameters->boundsRadius = BoidVariableParameters.boundsRadius;

	PassParameters->randSeed = FMath::Rand() % (INT32_MAX + 1);

	TShaderMapRef<FBoidsRPInitExampleCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	FIntVector GroupCounts = FIntVector(FMath::DivideAndRoundUp(BoidConstantParameters.numBoids, BoidsExample_ThreadsPerGroup), 1, 1);

	FComputeShaderUtils::AddPass(GraphBuilder,
		RDG_EVENT_NAME("RP_InitBoidsExample"),
		ERDGPassFlags::Compute | ERDGPassFlags::NeverCull,
		ComputeShader,
		PassParameters,
		GroupCounts);

	GraphBuilder.QueueBufferExtraction(BoidsPingPongBuffer.WriteScopedRef, &BoidsPingPongBuffer.ReadPooled);
	GraphBuilder.Execute();

	/*BoidsPingPongBuffer.PingPong(GraphBuilder);*/
}

// ____________________________________________ Execute Compute Shader

void AComputeRPEmitter::ExecuteComputeShader(float DeltaTime)
{
	check(IsInGameThread());

	LastDeltaTime = DeltaTime;

	BoidVariableParameters.boundsRadius = BoundsConstantParameters.Radius;
	BoidVariableParameters.transformMatrix = (FMatrix44f)BoundsMatrix;
	BoidVariableParameters.inverseTransformMatrix = (FMatrix44f)BoundsMatrix.Inverse();

	SetNiagaraVariableParameters();
}

void AComputeRPEmitter::ExecuteComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	FRDGBuilder GraphBuilder(RHICmdList);

	QUICK_SCOPE_CYCLE_COUNTER(STAT_ComputeRPExample_Dispatch); // Used to gather CPU profiling data for Unreal Insights.
	SCOPED_DRAW_EVENT(GraphBuilder.RHICmdList, ComputeRPExample_Compute); // Used to profile GPU activity and add metadata to be consumed by for example RenderDoc

	FString OwnerName = "ComputeRPEmitter";
	const TCHAR* SRVName = *(FString(OwnerName) + TEXT("BoidsIn_StructuredBuffer"));
	const TCHAR* UAVName = *(FString(OwnerName) + TEXT("BoidsOut_StructuredBuffer"));
	BoidsPingPongBuffer.RegisterRW(GraphBuilder, SRVName, UAVName);

	FBoidsRPUpdateExampleCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FBoidsRPUpdateExampleCS::FParameters>();
	PassParameters->numBoids = BoidConstantParameters.numBoids;
	PassParameters->deltaTime = LastDeltaTime * BoidVariableParameters.simulationSpeed;
	PassParameters->boidsIn = BoidsPingPongBuffer.ReadScopedSRV;
	PassParameters->boidsOut = BoidsPingPongBuffer.WriteScopedUAV;

	PassParameters->boundsInverseMatrix = BoidVariableParameters.inverseTransformMatrix;
	PassParameters->boundsMatrix = BoidVariableParameters.transformMatrix;
	PassParameters->boundsRadius = BoidVariableParameters.boundsRadius;

	PassParameters->minSpeed = BoidVariableParameters.minSpeed();
	PassParameters->maxSpeed = BoidVariableParameters.maxSpeed;
	PassParameters->turnSpeed = BoidVariableParameters.turnSpeed();
	PassParameters->minDistance = BoidVariableParameters.minDistance;
	PassParameters->minDistanceSq = BoidVariableParameters.minDistanceSq();
	PassParameters->cohesionFactor = BoidVariableParameters.cohesionFactor;
	PassParameters->separationFactor = BoidVariableParameters.separationFactor;
	PassParameters->alignmentFactor = BoidVariableParameters.alignmentFactor;

	TShaderMapRef<FBoidsRPUpdateExampleCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	FIntVector GroupCounts = FIntVector(FMath::DivideAndRoundUp(BoidConstantParameters.numBoids, BoidsExample_ThreadsPerGroup), 1, 1);
	FComputeShaderUtils::AddPass(GraphBuilder,
		RDG_EVENT_NAME("RP_BoidsUpdateExample"),
		ERDGPassFlags::Compute | ERDGPassFlags::NeverCull,
		ComputeShader,
		PassParameters,
		GroupCounts);

	//AddCopyBufferPass(GraphBuilder, BoidsPingPongBuffer.WriteScopedRef, BoidsPingPongBuffer.ReadScopedRef);
	GraphBuilder.QueueBufferExtraction(BoidsPingPongBuffer.WriteScopedRef, &BoidsPingPongBuffer.ReadPooled);
	GraphBuilder.Execute();
}


bool AComputeRPEmitter::SetNiagaraConstantParameters()
{
	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return false;
	}

	// Get the parameter store of the Niagara component
	FNiagaraUserRedirectionParameterStore& ParameterStore = Niagara->GetOverrideParameters();
	FNiagaraVariable SBParameter = FNiagaraVariable(FNiagaraTypeDefinition(UNiagaraDataInterfaceStructuredBuffer::StaticClass()), TEXT("boidsIn"));
	UNiagaraDataInterfaceStructuredBuffer* data = (UNiagaraDataInterfaceStructuredBuffer*)ParameterStore.GetDataInterface(SBParameter);

	if (data)
	{
		data->numBoids = BoidConstantParameters.numBoids;
		data->SetBuffer(nullptr);
	}

	Niagara->SetIntParameter("numBoids", BoidConstantParameters.numBoids);

	return true;
}

void AComputeRPEmitter::SetNiagaraVariableParameters()
{
	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return;
	}

	// Get the parameter store of the Niagara component
	FNiagaraUserRedirectionParameterStore& ParameterStore = Niagara->GetOverrideParameters();
	FNiagaraVariable SBParameter = FNiagaraVariable(FNiagaraTypeDefinition(UNiagaraDataInterfaceStructuredBuffer::StaticClass()), TEXT("boidsIn"));
	UNiagaraDataInterfaceStructuredBuffer* data = (UNiagaraDataInterfaceStructuredBuffer*)ParameterStore.GetDataInterface(SBParameter);

	if (data)
	{
		data->numBoids = BoidConstantParameters.numBoids;

		if (!data->GetBuffer().IsValid()
			&& BoidsPingPongBuffer.ReadPooled.IsValid())
		{
			data->SetBuffer(BoidsPingPongBuffer.ReadPooled);
		}
	}

	Niagara->SetFloatParameter("meshScale", BoidVariableParameters.meshScale);
	Niagara->SetFloatParameter("worldScale", BoidVariableParameters.worldScale);
}