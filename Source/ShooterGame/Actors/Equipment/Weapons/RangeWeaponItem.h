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

	void StartAiming();
	void StopAiming();
	
	FORCEINLINE float GetAimFOV() const { return AimFOV; }
	
	FORCEINLINE float GetAimingMaxMovementSpeed() const { return AimMovementMaxSpeed; }

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Properties", meta=(ToolTip = "Bullet spread half angle in degrees", UIMin = 0.0f, UIMax = 10.0f))
	float SpreadAngle = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Properties|Aim", DisplayName = "Spread Angle", meta=(UIMin = 0.0f))
	float AimSpreadAngle = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Properties|Aim", DisplayName = "Max Movement Speed")
	float AimMovementMaxSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Properties|Aim", DisplayName = "Field Of View")
	float AimFOV = 60.f;

private:
	float GetCurrentBulletSpreadAngle() const;
	
	void Shot() const;

	FVector GetBulletSpreadOffset(const float Angle, const FRotator ShotRotation) const;
	
	float GetFireTimerInterval() const { return 60.f / FireRate; };
	
	float PlayAnimMontage(UAnimMontage* Montage, float InPlayRate = 1.f) const;

	FTimerHandle FireTimerHandle;
	
	bool bIsAiming = false;
};
