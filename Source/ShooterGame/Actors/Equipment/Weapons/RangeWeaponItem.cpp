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

	EquippedSocketName = Socket::Weapon_R;
}

void ARangeWeaponItem::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	CharacterOwner = StaticCast<ABaseCharacter*>(NewOwner);
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

bool ARangeWeaponItem::CheckReloadRequiredForCharacter(const ABaseCharacter* InCharacter)
{
	if (!CanShoot())
	{
		StopFire();
		if (CurrentAmmo == 0 && bAutoReload)
		{
			InCharacter->ReloadEquippedWeapon();
		}
		return true;
	}
	return false;
}

void ARangeWeaponItem::Shot()
{
	if (CheckReloadRequiredForCharacter(CharacterOwner.Get())) return;
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
	const FVector ViewDirection = ViewRotation.RotateVector(FVector::ForwardVector);
	
	WeaponBarrel->Shot(ViewPoint, ViewDirection, PlayerController, GetCurrentBulletSpreadAngle());
	
	if (!bInfiniteAmmo)
	{
		CurrentAmmo--;
		SetAmmo(CurrentAmmo);
	}

	if (CheckReloadRequiredForCharacter(CharacterOwner.Get())) return;

	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ARangeWeaponItem::OnShotTimerElapsed, GetFireTimerInterval(), false);
	
}

void ARangeWeaponItem::OnShotTimerElapsed()
{
	if (!bIsFiring)
	{
		return;
	}

	switch (WeaponFireMode)
	{
		case EWeaponFireMode::WFM_Single:
		{
			StopFire(); break;
		}
		case EWeaponFireMode::WFM_Auto:
		{
			Shot();
		}
		default: break;
	}
}

float ARangeWeaponItem::GetFireTimerInterval() const
{
	return 60.f / GetCurrentFireRate();
}

void ARangeWeaponItem::StartFire()
{
	if(GetWorldTimerManager().IsTimerActive(FireTimerHandle))
	{
		return;
	}
	
	bIsFiring = true;
	Shot();
}

void ARangeWeaponItem::StopFire()
{
	bIsFiring = false;
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
	if (bIsReloading)
	{
		return;
	}

	bIsReloading = true;
	
	if (IsValid(CharacterReloadMontage))
	{
		const float MontageDuration = CharacterOwner->PlayAnimMontage(CharacterReloadMontage);
		PlayAnimMontage(ReloadMontage);
		if (WeaponReloadFormat == EWeaponReloadFormat::WRF_FullClip)
		{
			GetWorldTimerManager().SetTimer(ReloadingTimerHandle, [&]()
			{
				StopReload(true);
			}, MontageDuration, false);
		}
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

	if (!IsSuccess)
	{
		CharacterOwner->StopAnimMontage(CharacterReloadMontage);
		StopAnimMontage(ReloadMontage);
	}

	if (WeaponReloadFormat == EWeaponReloadFormat::WRF_ByBullet)
	{
		UAnimInstance* CharacterAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
		if(IsValid(CharacterAnimInstance))
		{
			CharacterAnimInstance->Montage_JumpToSection(MontageSection::EndReloading, CharacterReloadMontage);
		}

		UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
		if (IsValid(WeaponAnimInstance))
		{
			WeaponAnimInstance->Montage_JumpToSection(MontageSection::EndReloading, ReloadMontage);
		}
	}

	bIsReloading = false;

	GetWorldTimerManager().ClearTimer(ReloadingTimerHandle);
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

void ARangeWeaponItem::StopAnimMontage(const UAnimMontage* Montage, const float BlendOut) const
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	if (IsValid(WeaponAnimInstance))
	{
		WeaponAnimInstance->Montage_Stop(BlendOut, Montage);
	}
}
