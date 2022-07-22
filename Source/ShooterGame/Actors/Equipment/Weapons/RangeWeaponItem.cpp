#include "RangeWeaponItem.h"

#include "ShooterGame/ShooterGameTypes.h"
#include "ShooterGame/Characters/BaseCharacter.h"
#include "ShooterGame/Components/WeaponComponents/WeaponBarrelComponent.h"

ARangeWeaponItem::ARangeWeaponItem()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	
	WeaponBarrel = CreateDefaultSubobject<UWeaponBarrelComponent>(TEXT("WeaponBarrel"));
	WeaponBarrel->SetupAttachment(WeaponMesh, Socket::Muzzle);
}

void ARangeWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	SetAmmo(MaxAmmo);
}

float ARangeWeaponItem::GetCurrentBulletSpreadAngle() const
{
	const float AngleDegrees = bIsAiming ? AimSpreadAngle : SpreadAngle;
	return FMath::DegreesToRadians(AngleDegrees);
}

float ARangeWeaponItem::GetCurrentFireRate() const
{
	return  bIsAiming ? AimingFireRate : FireRate;
}

void ARangeWeaponItem::Shot()
{
	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("ARangeWeaponItem: Owner must be a ABaseCharacter"));
	ABaseCharacter* CharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());

	if (!CanShoot())
	{
		StopFire();
		if (CurrentAmmo == 0 && bAutoReload)
		{
			CharacterOwner->ReloadEquippedWeapon();
		}
		return;
	}

	StopReload(false);
	
	CharacterOwner->PlayAnimMontage(CharacterFireMontage);
	PlayAnimMontage(FireMontage);
	
	APlayerController* PlayerController = CharacterOwner->GetController<APlayerController>();
	if (!IsValid(PlayerController))
	{
		return;
	}
	FVector ViewPoint;
	FRotator ViewRotation;
	PlayerController->GetPlayerViewPoint(ViewPoint, ViewRotation);
	FVector ViewDirection = ViewRotation.RotateVector(FVector::ForwardVector);
	ViewDirection += GetBulletSpreadOffset(FMath::RandRange(0.f, GetCurrentBulletSpreadAngle()), ViewRotation);
	
	WeaponBarrel->Shot(ViewPoint, ViewDirection, PlayerController);
	
	if (!bInfiniteAmmo)
	{
		CurrentAmmo--;
		SetAmmo(CurrentAmmo);
	}
}

FVector ARangeWeaponItem::GetBulletSpreadOffset(const float Angle, const FRotator ShotRotation) const
{
	const float SpreadSize = FMath::Tan(Angle);
	const float RotationAngle = FMath::RandRange(0.f, 2 * PI);
	const float SpreadY = FMath::Cos(RotationAngle);
	const float SpreadZ = FMath::Sin(RotationAngle);

	return (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ + ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;
}

float ARangeWeaponItem::GetFireTimerInterval() const
{
	return 60.f / GetCurrentFireRate();
}

void ARangeWeaponItem::StartFire()
{
	Shot();
	if (WeaponFireMode == EWeaponFireMode::WFM_Auto)
	{
		GetWorldTimerManager().ClearTimer(FireTimerHandle);
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ARangeWeaponItem::Shot, GetFireTimerInterval(), true);
	}
}

void ARangeWeaponItem::StopFire()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ARangeWeaponItem::StartAiming()
{
	bIsAiming = true;
	WeaponBarrel->SetIsAiming(true);
}

void ARangeWeaponItem::StopAiming()
{
	bIsAiming = false;
	WeaponBarrel->SetIsAiming(false);
}

void ARangeWeaponItem::StartReload()
{
	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("ARangeWeaponItem: Owner must be a ABaseCharacter"));
	ABaseCharacter* CharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());

	bIsReloading = true;
	
	if (IsValid(CharacterReloadMontage))
	{
		const float MontageDuration = CharacterOwner->PlayAnimMontage(CharacterReloadMontage);
		PlayAnimMontage(ReloadMontage);
		GetWorldTimerManager().SetTimer(ReloadingTimerHandle, [&]()
		{
			StopReload(true);
		}, MontageDuration, false);
	}
	else
	{
		StopReload(true);
	}
}

void ARangeWeaponItem::StopReload(bool IsSuccess)
{
	if (!bIsReloading)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(ReloadingTimerHandle);
	
	bIsReloading = false;
	if (IsSuccess && OnReloadCompleteDelegate.IsBound())
	{
		OnReloadCompleteDelegate.Broadcast();
	}
}

void ARangeWeaponItem::SetAmmo(const int32 NewAmmo)
{
	CurrentAmmo = NewAmmo;
	if (OnAmmoChangedDelegate.IsBound())
	{
		OnAmmoChangedDelegate.Broadcast(CurrentAmmo);
	}
}

bool ARangeWeaponItem::CanShoot() const
{
	return CurrentAmmo > 0 || bInfiniteAmmo;
}

FTransform ARangeWeaponItem::GetForeGripTransform() const
{
	return WeaponMesh->GetSocketTransform(Socket::ForeGrip);
}


float ARangeWeaponItem::PlayAnimMontage(UAnimMontage* Montage, float InPlayRate) const
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	float Result = 0.f;
	if (IsValid(WeaponAnimInstance))
	{
		Result = WeaponAnimInstance->Montage_Play(Montage, InPlayRate);
	}
	return Result;
}
