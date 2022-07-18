#include "InteractiveActor.h"

#include "Components/CapsuleComponent.h"
#include "ShooterGame/Characters/BaseCharacter.h"

void AInteractiveActor::BeginPlay()
{
	Super::BeginPlay();
	if(IsValid(InteractionVolume))
	{
		InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &AInteractiveActor::OnInteractionVolumeBeginOverlap);
		InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &AInteractiveActor::OnInteractionVolumeEndOverlap);
	}
}

void AInteractiveActor::OnInteractionVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CharacterCapsuleSameAs(OtherActor, OtherComp))
	{
		ABaseCharacter* BaseCharacter = StaticCast<ABaseCharacter*>(OtherActor);
		BaseCharacter->RegisterInteractiveActor(this);
	}
}

void AInteractiveActor::OnInteractionVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CharacterCapsuleSameAs(OtherActor, OtherComp))
	{
		ABaseCharacter* BaseCharacter = StaticCast<ABaseCharacter*>(OtherActor);
		BaseCharacter->UnregisterInteractiveActor(this);
	}
}

bool AInteractiveActor::CharacterCapsuleSameAs(AActor* OtherActor, UPrimitiveComponent* OtherComp) const
{
	const ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);
	if(!IsValid(BaseCharacter))
	{
		return false;
	}

	if (Cast<UCapsuleComponent>(OtherComp) != BaseCharacter->GetCapsuleComponent())
	{
		return false;
	}
	
	return true;
}
