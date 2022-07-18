#include "ExternalCamera.h"
#include "Kismet/GameplayStatics.h"

AExternalCamera::AExternalCamera()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Camera interaction volume"));
	BoxComponent->SetBoxExtent(FVector(500.f, 500.f, 500.f));
	BoxComponent->SetCollisionObjectType(ECC_WorldDynamic);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComponent->SetupAttachment(RootComponent);
}

void AExternalCamera::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AExternalCamera::OnBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AExternalCamera::OnEndOverlap);
}

void AExternalCamera::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerController *PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FViewTargetTransitionParams CameraTransitionParams;
	CameraTransitionParams.BlendTime = TransitionToCameraViewTime;
	PlayerController->SetViewTarget(this, CameraTransitionParams);
}

void AExternalCamera::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerController *PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APawn *Pawn = PlayerController->GetPawn();
	FViewTargetTransitionParams PawnTransitionParams;
	PawnTransitionParams.BlendTime = TransitionToPawnViewTime;
	PawnTransitionParams.bLockOutgoing = true;
	PlayerController->SetViewTarget(Pawn, PawnTransitionParams);
}
