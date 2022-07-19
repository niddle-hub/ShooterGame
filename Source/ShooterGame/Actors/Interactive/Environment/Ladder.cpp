#include "Ladder.h"
#include "Components/BoxComponent.h"
#include "ShooterGame/ShooterGameTypes.h"

ALadder::ALadder()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Ladder Root"));
	
	LeftRailMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Left Rail"));
	LeftRailMeshComponent->SetupAttachment(RootComponent);

	RightRailMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Right Rail"));
	RightRailMeshComponent->SetupAttachment(RootComponent);

	StepMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Ladder Step"));
	StepMeshComponent->SetupAttachment(RootComponent);

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Volume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfile::InteractOnlyPawn);
	InteractionVolume->SetGenerateOverlapEvents(true);

	TopInteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Top Down Interaction Volume"));
	TopInteractionVolume->SetupAttachment(RootComponent);
	TopInteractionVolume->SetCollisionProfileName(CollisionProfile::InteractOnlyPawn);
	TopInteractionVolume->SetGenerateOverlapEvents(true);
}

void ALadder::BeginPlay()
{
	Super::BeginPlay();

	TopInteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OnInteractionVolumeBeginOverlap);
	TopInteractionVolume->OnComponentEndOverlap.AddDynamic(this, &ALadder::OnInteractionVolumeEndOverlap);
}

void ALadder::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	LeftRailMeshComponent->SetRelativeLocation(FVector(0.f, LadderWidth * 0.5f, LadderHeight * 0.5f));
	RightRailMeshComponent->SetRelativeLocation(FVector(0.f, -LadderWidth * 0.5f, LadderHeight * 0.5f));

	SetRailMeshScaleZ(LeftRailMeshComponent);
	SetRailMeshScaleZ(RightRailMeshComponent);

	const UStaticMesh* StepMesh = StepMeshComponent->GetStaticMesh();
	if (IsValid(StepMesh))
	{
		const float MeshWidth = StepMesh->GetBoundingBox().GetSize().Y;
		if (!FMath::IsNearlyZero(MeshWidth))
		{
			StepMeshComponent->SetRelativeScale3D(FVector(1.f, LadderWidth / MeshWidth, 1.f));
		}
	}

	StepMeshComponent->ClearInstances();
	const uint32 StepsCount = FMath::FloorToInt((LadderHeight - BottomStepOffset) / StepsInterval);
	for (uint32 i = 0; i < StepsCount; ++i)
	{
		FTransform InstanceTransform(FVector(1.f, 1.f, BottomStepOffset + i * StepsInterval));
		StepMeshComponent->AddInstance(InstanceTransform);
	}

	const float BoxDepthExtent = GetLadderInteractionBox()->GetUnscaledBoxExtent().X;
	GetLadderInteractionBox()->SetBoxExtent(FVector(BoxDepthExtent, LadderWidth * 0.5f, LadderHeight * 0.5f));
	GetLadderInteractionBox()->SetRelativeLocation(FVector(BoxDepthExtent, 0.f, LadderHeight * 0.5f));

	const FVector TopBoxExtent = TopInteractionVolume->GetUnscaledBoxExtent();
	TopInteractionVolume->SetBoxExtent(FVector(TopBoxExtent.X, LadderWidth * 0.5f, TopBoxExtent.Z));
	TopInteractionVolume->SetRelativeLocation(FVector(-TopBoxExtent.X, 0.f, LadderHeight + TopBoxExtent.Z));
}

FVector ALadder::GetAttachFromTopAnimMontageStartLocation() const
{
	const FRotator LadderOrientation = GetActorForwardVector().ToOrientationRotator();
	const FVector Offset = LadderOrientation.RotateVector(AttachFromTopInitialOffset);
	const FVector LadderTop = GetActorLocation() + GetActorUpVector() * LadderHeight;
	
	return LadderTop + Offset;
}

UBoxComponent* ALadder::GetLadderInteractionBox() const
{
	return StaticCast<UBoxComponent*>(InteractionVolume);
}

void ALadder::OnInteractionVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnInteractionVolumeBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (!CharacterCapsuleSameAs(OtherActor, OtherComp))
	{
		return;
	}
	
	if (OverlappedComponent == TopInteractionVolume)
	{
		OnTop = true;
	}
}

void ALadder::OnInteractionVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnInteractionVolumeEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (!CharacterCapsuleSameAs(OtherActor, OtherComp))
	{
		return;
	}
	
	if (OverlappedComponent == TopInteractionVolume)
	{
		OnTop = false;
	}
}

void ALadder::SetRailMeshScaleZ(UStaticMeshComponent* RailMeshComponent) const
{
	const UStaticMesh* RailMesh = RailMeshComponent->GetStaticMesh();
	if (IsValid(RailMesh))
	{
		const float MeshHeight = RailMesh->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHeight))
		{
			RailMeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, LadderHeight/MeshHeight));
		}
	}
}
