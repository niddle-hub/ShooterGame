#include "CharacterEquipmentComponent.h"

#include "ShooterGame/ShooterGameTypes.h"
#include "ShooterGame/Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "ShooterGame/Characters/BaseCharacter.h"

EEquipableItemType UCharacterEquipmentComponent::GetEquippedItemType() const
{
	EEquipableItemType Result = EEquipableItemType::EIT_None;
	if (IsValid(EquippedWeapon))
	{
		Result = EquippedWeapon->GetItemType();
	}

	return Result;
}

void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("CharacterEquipmentComponent can be used only with a ABaseCharacter"));
	OwnerCharacter = StaticCast<ABaseCharacter*>(GetOwner());
	CreateLoadout();
}

void UCharacterEquipmentComponent::CreateLoadout()
{
	if (!IsValid(RangeWeaponItemClass))
	{
		return;
	}
	
	EquippedWeapon = GetWorld()->SpawnActor<ARangeWeaponItem>(RangeWeaponItemClass);
	EquippedWeapon->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Socket::Weapon_R);
	EquippedWeapon->SetOwner(OwnerCharacter.Get());
}
