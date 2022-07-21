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
	const FVector ViewDirection = ViewRotation.RotateVector(FVector::ForwardVector);
	
	WeaponBarrel->Shot(ViewPoint, ViewDirection, PlayerController);
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
