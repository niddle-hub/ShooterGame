#include "AnimNotify_LaunchThrowable.h"
#include "ShooterGame/Characters/BaseCharacter.h"
#include "ShooterGame/Components/CharacterComponents/CharacterEquipmentComponent.h"

void UAnimNotify_LaunchThrowable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	const ABaseCharacter* Owner = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (!IsValid(Owner))
	{
		return;
	}
	Owner->GetCharacterEquipmentComponent_Mutable()->LaunchEquippedThrowableItem();
}
