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

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoChangedSignature, int32);

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

	FORCEINLINE int32 GetCurrenAmmo() const { return CurrentAmmo; }
	FORCEINLINE int32 GetMaxAmmo() const { return MaxAmmo; }

	EAmmunitionType GetAmmoType() const { return AmmoType; }	

	void SetAmmo(const int32 NewAmmo);

	bool CanShoot() const;

	bool HasInfiniteAmmo() const { return bInfiniteAmmo; }
	
	FORCEINLINE float GetAimFOV() const { return AimFOV; }
	
	FORCEINLINE float GetAimingMaxMovementSpeed() const { return AimMovementMaxSpeed; }

	FORCEINLINE float GetAimTurnModifier() const { return AimTurnModifier; }

	FORCEINLINE float GetAimLookUpModifier() const { return AimLookUpModifier; }

	FTransform GetForeGripTransform() const;

	FOnAmmoChangedSignature OnAmmoChanged;
	
protected:
	virtual void BeginPlay() override;
	
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Properties|NoScope", meta=(ToolTip = "Rate in rounds per minute", UIMin = 1.0f))
	float FireRate = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Properties|NoScope", meta=(ToolTip = "Bullet spread half angle in degrees", UIMin = 0.0f, UIMax = 10.0f))
	float SpreadAngle = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Properties|Aim", meta=(ToolTip = "Rate in rounds per minute in Aiming mode", UIMin = 1.0f))
	float AimingFireRate = 250.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Properties|Aim", DisplayName = "Spread Angle", meta=(UIMin = 0.0f))
	float AimSpreadAngle = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Properties|Aim", DisplayName = "Max Movement Speed")
	float AimMovementMaxSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Properties|Aim", DisplayName = "Field Of View")
	float AimFOV = 60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Properties|Aim", DisplayName = "Turn Modifier" , meta=(UIMin = 0.0f, UIMax = 1.0f))
	float AimTurnModifier = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Properties|Aim", DisplayName = "LookUp Modifier", meta=(UIMin = 0.0f, UIMax = 1.0f))
	float AimLookUpModifier = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Properties|Ammo")
	bool bInfiniteAmmo = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Properties|Ammo")
	bool bAutoReload = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Properties|Ammo")
	EAmmunitionType AmmoType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RangeWeapon|Properties|Ammo", meta=(UIMin=0))
	int32 MaxAmmo = 30;

private:
	int32 CurrentAmmo = 0;
	
	float GetCurrentBulletSpreadAngle() const;

	float GetCurrentFireRate() const;
	
	void Shot();

	FVector GetBulletSpreadOffset(const float Angle, const FRotator ShotRotation) const;
	
	float GetFireTimerInterval() const;
	
	float PlayAnimMontage(UAnimMontage* Montage, float InPlayRate = 1.f) const;

	FTimerHandle FireTimerHandle;
	
	bool bIsAiming = false;
};
