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

float ARangeWeaponItem::GetCurrentBulletSpreadAngle() const
{
	const float AngleDegrees = bIsAiming ? AimSpreadAngle : SpreadAngle;
	return FMath::DegreesToRadians(AngleDegrees);
}

void ARangeWeaponItem::Shot() const
{
	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("ARangeWeaponItem: Owner must be a ABaseCharacter"));
	ABaseCharacter* CharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());
	
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
}

FVector ARangeWeaponItem::GetBulletSpreadOffset(const float Angle, const FRotator ShotRotation) const
{
	const float SpreadSize = FMath::Tan(Angle);
	const float RotationAngle = FMath::RandRange(0.f, 2 * PI);
	const float SpreadY = FMath::Cos(RotationAngle);
	const float SpreadZ = FMath::Sin(RotationAngle);

	return (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ + ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;
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
}

void ARangeWeaponItem::StopAiming()
{
	bIsAiming = false;
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
