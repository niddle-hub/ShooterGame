#pragma once

#include "CoreMinimal.h"
#include "ShooterGame/Actors/Equipment/EquipableItem.h"
#include "RangeWeaponItem.generated.h"

UCLASS(Blueprintable)
class SHOOTERGAME_API ARangeWeaponItem : public AEquipableItem
{
	GENERATED_BODY()

public:
	ARangeWeaponItem();

	void Fire() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RangeWeapon")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Animations")
	UAnimMontage* FireMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Animations")
	UAnimMontage* CharacterFireMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RangeWeapon|Components")
	class UWeaponBarrelComponent* WeaponBarrel;

private:
	float PlayAnimMontage(UAnimMontage* Montage, float InPlayRate = 1.f) const;
	
	
};
