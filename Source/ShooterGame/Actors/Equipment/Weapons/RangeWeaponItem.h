#pragma once

#include "CoreMinimal.h"
#include "ShooterGame/Actors/Equipment/EquipableItem.h"
#include "RangeWeaponItem.generated.h"

UENUM(BlueprintType)
enum class EWeaponFireMode : uint8
{
	WFM_Single UMETA(DisplayName = "Single"),
	WFM_Burst UMETA(DisplayName = "Burst"),
	WFM_Auto UMETA(DisplayName = "Auto"),
	WFM_MAX UMETA(Hidden)
};

UCLASS(Blueprintable)
class SHOOTERGAME_API ARangeWeaponItem : public AEquipableItem
{
	GENERATED_BODY()

public:
	ARangeWeaponItem();

	void StartFire();
	void StopFire();

	FTransform GetForeGripTransform() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RangeWeapon")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Animations")
	UAnimMontage* FireMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Animations")
	UAnimMontage* CharacterFireMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RangeWeapon|Components")
	class UWeaponBarrelComponent* WeaponBarrel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Properties")
	EWeaponFireMode WeaponFireMode = EWeaponFireMode::WFM_Single;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Properties", meta=(ToolTip = "Rate in rounds per minute", UIMin = 1.0f))
	float FireRate = 300.f;

private:
	void Shot() const;
	
	float GetFireTimerInterval() const { return 60.f / FireRate; };
	
	float PlayAnimMontage(UAnimMontage* Montage, float InPlayRate = 1.f) const;

	FTimerHandle FireTimerHandle;
	
	
};
