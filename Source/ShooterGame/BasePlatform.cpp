#include "BasePlatform.h"

ABasePlatform::ABasePlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* DefaultPlatformRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Platform root"));
	RootComponent = DefaultPlatformRoot;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform"));
	PlatformMesh->SetupAttachment(DefaultPlatformRoot);
}

void ABasePlatform::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = PlatformMesh->GetRelativeLocation();

	if (IsValid(TimelineCurve))
	{
		FOnTimelineFloatStatic PlatformMovementTimelineUpdate;
		PlatformMovementTimelineUpdate.BindUObject(this, &ABasePlatform::PlatformTimelineUpdate);
		PlatformTimeline.AddInterpFloat(TimelineCurve, PlatformMovementTimelineUpdate);
	}

	if (IsValid(PlatformInvocator))
	{
		PlatformInvocator->OnInvocatorActivated.AddDynamic(this, &ABasePlatform::OnPlatformInvoked);
	}
}

void ABasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlatformTimeline.TickTimeline(DeltaTime);
}

void ABasePlatform::BeginPlatformElevate()
{
	if (PlatformBehavior == EPlatformBehavior::OnDemand)
	{
		PlatformTimeline.Play();
	}

	if (PlatformBehavior == EPlatformBehavior::Loop)
	{
		PlatformTimeline.SetLooping(true);
		PlatformTimeline.Play();
	}
}

void ABasePlatform::EndPlatformElevate()
{
	PlatformTimeline.Reverse();
}

void ABasePlatform::OnPlatformInvoked()
{
	BeginPlatformElevate();
}

void ABasePlatform::PlatformTimelineUpdate(const float Alpha) const
{
	const FVector PlatformTargetLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	PlatformMesh->SetRelativeLocation(PlatformTargetLocation);
}

