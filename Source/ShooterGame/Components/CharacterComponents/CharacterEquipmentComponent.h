#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShooterGame/ShooterGameTypes.h"
#include "CharacterEquipmentComponent.generated.h"

typedef TArray<int32, TInlineAllocator<static_cast<uint32>(EAmmunitionType::AT_MAX)>> TAmmunitionArray;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEquippedWeaponAmmoChangedSignature, int32, int32);

class ARangeWeaponItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERGAME_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	EEquipableItemType GetEquippedItemType() const;

	FORCEINLINE ARangeWeaponItem* GetEquippedRangeWeapon() const { return EquippedWeapon; }

	FOnEquippedWeaponAmmoChangedSignature OnEquippedWeaponAmmoChangedDelegate;

	void ReloadEquippedWeapon() const;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<ARangeWeaponItem> RangeWeaponItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TMap<EAmmunitionType, int32> MaxAmmunitionAmount;
	
private:
	TAmmunitionArray AmmunitionArray;
	
	void CreateLoadout();
	ARangeWeaponItem* EquippedWeapon;

	UFUNCTION()
	void OnEquippedWeaponAmmoChanged(int32 NewAmmo) const;

	UFUNCTION()
	void OnEquippedWeaponReloaded();

	TWeakObjectPtr<class ABaseCharacter> OwnerCharacter;

	int32 GetAvailableAmmunitionForEquippedWeapon() const;
};
