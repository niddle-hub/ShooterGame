#include "AnimNotify_ReloadAmmo.h"
#include "ShooterGame/Characters/BaseCharacter.h"
#include "ShooterGame/Components/CharacterComponents/CharacterEquipmentComponent.h"

void UAnimNotify_ReloadAmmo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	const ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (!IsValid(Character))
	{
		return;
	}
	Character->GetCharacterEquipmentComponent_Mutable()->ReloadAmmoInEquippedWeapon(NumberOfAmmo, true);
}
