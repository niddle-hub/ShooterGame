#include "Zipline.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ShooterGame/ShooterGameTypes.h"

AZipline::AZipline()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Zipline Root"));
	
	StartPillarMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Start Pillar"));
	StartPillarMeshComponent->SetupAttachment(RootComponent);

	EndPillarMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("End Pillar"));
	EndPillarMeshComponent->SetupAttachment(RootComponent);
	
	CableMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cable"));
	CableMeshComponent->SetupAttachment(RootComponent);

	InteractionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Interaction Volume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(OverlapCollisionProfilePawnInteraction);
	InteractionVolume->SetGenerateOverlapEvents(true);
}

void AZipline::BeginPlay()
{
	Super::BeginPlay();
}

void AZipline::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	StartPillarMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, ZiplinePillarHeight * 0.5f));
	EndPillarMeshComponent->SetRelativeLocation(FVector(ZiplineLenght, 0.f, ZiplinePillarHeight * 0.5f + EndPillarOffsetZ));
	
	SetPillarMeshScaleZ(StartPillarMeshComponent);
	SetPillarMeshScaleZ(EndPillarMeshComponent);

	const FVector StartPillarTopLocation = FVector::UpVector * (ZiplinePillarHeight - CableTopOffset);
	const FVector EndPillarTopLocation = FVector(ZiplineLenght, 0.f, ZiplinePillarHeight + EndPillarOffsetZ - CableTopOffset);
	
	const float TargetCableLenght = FVector::Dist(StartPillarTopLocation, EndPillarTopLocation);
	CableLenght = TargetCableLenght;
	const FRotator TargetCableRotation = UKismetMathLibrary::FindLookAtRotation(StartPillarTopLocation, EndPillarTopLocation);

	CableTopOffset = FMath::Clamp(CableTopOffset, 0.f, ZiplinePillarHeight);
	CableMeshComponent->SetRelativeRotation(TargetCableRotation);
	CableMeshComponent->SetRelativeLocation(FVector(ZiplineLenght * 0.5f, 0.f, ZiplinePillarHeight - CableTopOffset + EndPillarOffsetZ * 0.5f));
	
	const UStaticMesh* CableMesh = CableMeshComponent->GetStaticMesh();
	if (IsValid(CableMesh))
	{
		const float MeshWidth = CableMesh->GetBoundingBox().GetSize().X;
		if (!FMath::IsNearlyZero(MeshWidth))
		{
			CableMeshComponent->SetRelativeScale3D(FVector(TargetCableLenght / MeshWidth, 1.f, 1.f));
		}
	}
	
	GetZiplineInteractionVolume()->SetRelativeLocation(CableMeshComponent->GetRelativeLocation());
	GetZiplineInteractionVolume()->SetRelativeRotation(FRotator(CableMeshComponent->GetRelativeRotation().Pitch + 90.f, 0.f, 0.f));
	GetZiplineInteractionVolume()->SetCapsuleHalfHeight(TargetCableLenght * 0.5);
}

UCapsuleComponent* AZipline::GetZiplineInteractionVolume() const
{
	return StaticCast<UCapsuleComponent*>(InteractionVolume);
}

void AZipline::OnInteractionVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnInteractionVolumeBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (!CharacterCapsuleSameAs(OtherActor, OtherComp))
	{
		return;
	}
	OnZipline = true;
}

void AZipline::OnInteractionVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnInteractionVolumeEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	OnZipline = false;
}

void AZipline::SetPillarMeshScaleZ(UStaticMeshComponent* PillarMeshComponent) const
{
	const UStaticMesh* PillarMesh = PillarMeshComponent->GetStaticMesh();
	if (IsValid(PillarMesh))
	{
		const float MeshHeight = PillarMesh->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHeight))
		{
			PillarMeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, ZiplinePillarHeight/MeshHeight));
		}
	}
}
