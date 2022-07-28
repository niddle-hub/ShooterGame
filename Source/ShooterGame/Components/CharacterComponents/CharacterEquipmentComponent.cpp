#include "CharacterEquipmentComponent.h"

#include "ShooterGame/ShooterGameTypes.h"
#include "ShooterGame/Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "ShooterGame/Characters/BaseCharacter.h"

void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("CharacterEquipmentComponent can be used only with a ABaseCharacter"));
	OwnerCharacter = StaticCast<ABaseCharacter*>(GetOwner());
	OnWeaponChangedDelegate.AddUObject(this, &UCharacterEquipmentComponent::OnWeaponChanged);
	CreateLoadout();
}

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

void UCharacterEquipmentComponent::UnequipCurrentItem()
{
	if (IsValid(EquippedItem))
	{
		EquippedItem->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, EquippedItem->GetUnequippedSocketName());
	}
	if (IsValid(EquippedWeapon))
	{
		EquippedWeapon->StopFire();
		EquippedWeapon->StopReload(false);
		EquippedWeapon->OnAmmoChangedDelegate.Remove(OnEquippedWeaponAmmoChangedHandle);
		EquippedWeapon->OnReloadCompleteDelegate.Remove(OnEquippedWeaponReloadedHandle);
	}
	EquippedSlot = EEquipmentSlot::ES_None;
}

void UCharacterEquipmentComponent::AttachEquippedItem() const
{
	EquippedItem->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, EquippedItem->GetEquippedSocketName());
}

void UCharacterEquipmentComponent::OnEquipMontageEnd()
{
	bIsEquipping = false;
	AttachEquippedItem();
}

void UCharacterEquipmentComponent::EquipItemInSlot(EEquipmentSlot Slot)
{
	if (bIsEquipping)
	{
		return;
	}
	UnequipCurrentItem();
	
	EquippedItem = ItemsArray[static_cast<uint32>(Slot)];
	EquippedWeapon = Cast<ARangeWeaponItem>(EquippedItem);

	if (IsValid(EquippedItem))
	{
		UAnimMontage* EquipMontage = EquippedItem->GetCharacterEquipAnimation();
		if (IsValid(EquipMontage))
		{
			bIsEquipping = true;
			const float EquipDuration = OwnerCharacter->PlayAnimMontage(EquipMontage);
			GetWorld()->GetTimerManager().SetTimer(EquipTimerHandle, this, &UCharacterEquipmentComponent::OnEquipMontageEnd, EquipDuration, false);
		}
		else
		{
			AttachEquippedItem();
		}
		EquippedSlot = Slot;
	}
	
	if (IsValid(EquippedWeapon))
	{
		OnEquippedWeaponAmmoChangedHandle = EquippedWeapon->OnAmmoChangedDelegate.AddUObject(this, &UCharacterEquipmentComponent::OnEquippedWeaponAmmoChanged);
		OnEquippedWeaponReloadedHandle = EquippedWeapon->OnReloadCompleteDelegate.AddUObject(this, &UCharacterEquipmentComponent::OnEquippedWeaponReloaded);
		OnEquippedWeaponAmmoChanged(EquippedWeapon->GetCurrentAmmo());
		OnWeaponChangedDelegate.IsBound() ? OnWeaponChangedDelegate.Broadcast() : 0;
	}
}

void UCharacterEquipmentComponent::EquipNext()
{	
	const uint32 CurrentSlotIndex = static_cast<uint32>(EquippedSlot);
	uint32 NextSlotIndex = NextItemsArraySlotIndex(CurrentSlotIndex);
	
	while (CurrentSlotIndex != NextSlotIndex && !IsValid(ItemsArray[NextSlotIndex]))
	{
		NextSlotIndex = NextItemsArraySlotIndex(NextSlotIndex);
	}
	if (CurrentSlotIndex != NextSlotIndex)
	{
		EquipItemInSlot(static_cast<EEquipmentSlot>(NextSlotIndex));
	}
}

void UCharacterEquipmentComponent::EquipPrevious()
{
	const uint32 CurrentSlotIndex = static_cast<uint32>(EquippedSlot);
	uint32 PrevSlotIndex = PreviousItemsArraySlotIndex(CurrentSlotIndex);
	
	while (CurrentSlotIndex != PrevSlotIndex && !IsValid(ItemsArray[PrevSlotIndex]))
	{
		PrevSlotIndex = PreviousItemsArraySlotIndex(PrevSlotIndex);
	}
	if (CurrentSlotIndex != PrevSlotIndex)
	{
		EquipItemInSlot(static_cast<EEquipmentSlot>(PrevSlotIndex));
	}
}

void UCharacterEquipmentComponent::OnWeaponChanged() const
{
	if (IsValid(EquippedWeapon))
	{
		EquippedWeapon->CheckReloadRequiredForCharacter(OwnerCharacter.Get());
	}
}

uint32 UCharacterEquipmentComponent::NextItemsArraySlotIndex(uint32 CurrentSlotIndex) const
{
	if (CurrentSlotIndex == ItemsArray.Num() - 1)
	{
		return 0;
	}
	return CurrentSlotIndex + 1;
}

uint32 UCharacterEquipmentComponent::PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex) const
{
	if (CurrentSlotIndex == 0)
	{
		return ItemsArray.Num() - 1;
	}
	return CurrentSlotIndex - 1;
}

void UCharacterEquipmentComponent::CreateLoadout()
{
	AmmunitionArray.AddZeroed(static_cast<uint32>(EAmmunitionType::AT_MAX));
	for (const TPair<EAmmunitionType, int32>& AmmoPair : MaxAmmunitionAmount)
	{
		AmmunitionArray[static_cast<uint32>(AmmoPair.Key)] = FMath::Max(AmmoPair.Value, 0);
	}

	ItemsArray.AddZeroed(static_cast<uint32>(EEquipmentSlot::ES_MAX));
	for (const TPair<EEquipmentSlot, TSubclassOf<AEquipableItem>>& ItemPair : ItemsLoadout)
	{
		if (!IsValid(ItemPair.Value))
		{
			continue;
		}
		AEquipableItem* Item = GetWorld()->SpawnActor<AEquipableItem>(ItemPair.Value);
		Item->SetOwner(OwnerCharacter.Get());
		Item->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Item->GetUnequippedSocketName());
		ItemsArray[static_cast<uint32>(ItemPair.Key)] = Item;
	}
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
	ReloadAmmoInEquippedWeapon();
}

void UCharacterEquipmentComponent::ReloadAmmoInEquippedWeapon(const int32 NumberOfAmmo, const bool CheckIsFull)
{
	int32 AvailableAmmunition = GetAvailableAmmunitionForEquippedWeapon();
	const int32 CurrentAmmo = EquippedWeapon->GetCurrentAmmo();
	const int32 AmmoToReload = EquippedWeapon->GetMaxAmmo() - CurrentAmmo;
	int32 ReloadedAmmo = FMath::Min(AvailableAmmunition, AmmoToReload);
	
	if (NumberOfAmmo > 0)
	{
		ReloadedAmmo = FMath::Min(ReloadedAmmo, NumberOfAmmo);
	}
	
	AmmunitionArray[static_cast<uint32>(EquippedWeapon->GetAmmoType())] -= ReloadedAmmo;
	EquippedWeapon->SetAmmo(ReloadedAmmo + CurrentAmmo);

	if (CheckIsFull)
	{
		AvailableAmmunition = GetAvailableAmmunitionForEquippedWeapon();
		if (AvailableAmmunition == 0 || EquippedWeapon->IsFullClip())
		{
			EquippedWeapon->StopReload(true);
		}
	}
	
}

int32 UCharacterEquipmentComponent::GetAvailableAmmunitionForEquippedWeapon() const
{
	check(GetEquippedRangeWeapon())
	return AmmunitionArray[static_cast<uint32>(GetEquippedRangeWeapon()->GetAmmoType())];
}
