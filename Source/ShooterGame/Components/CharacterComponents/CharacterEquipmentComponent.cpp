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

void UCharacterEquipmentComponent::ReloadEquippedWeapon() const
{
	checkf(IsValid(EquippedWeapon), TEXT("Trying to reload an equipped weapon but there is no weapon equipped"));

	const bool ShouldSkipReload = EquippedWeapon->HasInfiniteAmmo() || EquippedWeapon->IsFullClip();
	if (ShouldSkipReload)
	{
		return;
	}
	
	const int32 AvailableAmmunition = GetAvailableAmmunitionForEquippedWeapon();
	if (AvailableAmmunition <= 0)
	{
		return;
	}

	EquippedWeapon->StartReload();
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

	AmmunitionArray.AddZeroed(static_cast<uint32>(EAmmunitionType::AT_MAX));
	for (const TPair<EAmmunitionType, int32>& AmmoPair : MaxAmmunitionAmount)
	{
		AmmunitionArray[static_cast<uint32>(AmmoPair.Key)] = FMath::Max(AmmoPair.Value, 0);
	}
	
	EquippedWeapon = GetWorld()->SpawnActor<ARangeWeaponItem>(RangeWeaponItemClass);
	EquippedWeapon->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Socket::Weapon_R);
	EquippedWeapon->SetOwner(OwnerCharacter.Get());
	EquippedWeapon->OnAmmoChangedDelegate.AddUObject(this, &UCharacterEquipmentComponent::OnEquippedWeaponAmmoChanged);
	EquippedWeapon->OnReloadCompleteDelegate.AddUObject(this, &UCharacterEquipmentComponent::OnEquippedWeaponReloaded);
	OnEquippedWeaponAmmoChanged(EquippedWeapon->GetCurrenAmmo());
}

void UCharacterEquipmentComponent::OnEquippedWeaponAmmoChanged(const int32 NewAmmo) const
{
	if (OnEquippedWeaponAmmoChangedDelegate.IsBound())
	{
		const int32 NewTotalAmmo = GetAvailableAmmunitionForEquippedWeapon();
		OnEquippedWeaponAmmoChangedDelegate.Broadcast(NewAmmo, NewTotalAmmo);
	}
}

void UCharacterEquipmentComponent::OnEquippedWeaponReloaded()
{
	const int32 AvailableAmmunition = GetAvailableAmmunitionForEquippedWeapon();
	const int32 CurrentAmmo = EquippedWeapon->GetCurrenAmmo();
	const int32 AmmoToReload = EquippedWeapon->GetMaxAmmo() - CurrentAmmo;
	const int32 ReloadedAmmo = FMath::Min(AvailableAmmunition, AmmoToReload);
	
	AmmunitionArray[static_cast<uint32>(EquippedWeapon->GetAmmoType())] -= ReloadedAmmo;
	EquippedWeapon->SetAmmo(ReloadedAmmo + CurrentAmmo);
}

int32 UCharacterEquipmentComponent::GetAvailableAmmunitionForEquippedWeapon() const
{
	check(GetEquippedRangeWeapon())
	return AmmunitionArray[static_cast<uint32>(GetEquippedRangeWeapon()->GetAmmoType())];
}
