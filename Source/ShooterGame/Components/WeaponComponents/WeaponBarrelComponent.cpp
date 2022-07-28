#include "WeaponBarrelComponent.h"

#include "DrawDebugHelpers.h"
#include "ShooterGame/ShooterGameTypes.h"

#if ENABLE_DRAW_DEBUG
	#include "Kismet/GameplayStatics.h"
	#include "ShooterGame/Subsystems/DebugSubsystem.h"
#endif

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/DecalComponent.h"

void UWeaponBarrelComponent::Shot(const FVector ShotStart, FVector ShotDirection, AController* Controller, float SpreadAngle) const
{
	for (int32 i = 0; i < BulletsPerShot; i ++)
	{
		ShotDirection += GetBulletSpreadOffset(FMath::RandRange(0.f, SpreadAngle), ShotDirection.ToOrientationRotator());
	
		const FVector MuzzleLocation = GetComponentLocation();
		FVector ShotEnd = ShotStart + GetCurrentFireRange() * ShotDirection;

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FireFX, MuzzleLocation, GetComponentRotation());

		FHitResult ShotResult;

#if ENABLE_DRAW_DEBUG
		const UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
		bool IsDebugEnabled = DebugSubsystem->IsEnabledCategory(Debug::RangeWeapon);
#else
		bool IsDebugEnabled = false;
#endif
	
		if(GetWorld()->LineTraceSingleByChannel(ShotResult, ShotStart, ShotEnd, ECC_Bullet))
		{
			ShotEnd = ShotResult.ImpactPoint;
			AActor* HitActor = ShotResult.GetActor();
			if (IsValid(HitActor))
			{
				float DamageAmount = GetCurrentDamage();
				if(IsValid(DamageCurve))
				{
					DamageAmount = GetCurrentDamage() * DamageCurve->GetFloatValue(ShotResult.Distance);
				}
				FPointDamageEvent DamageEvent(DamageAmount, ShotResult, ShotDirection, DamageType);
				HitActor->TakeDamage(DamageAmount, DamageEvent, Controller, GetOwner());
			}

			UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultDecal.DecalMaterial, DefaultDecal.DecalSize, ShotResult.ImpactPoint, ShotResult.ImpactNormal.ToOrientationRotator());
			if (IsValid(DecalComponent))
			{
				DecalComponent->SetFadeScreenSize(0.001f);
				DecalComponent->SetFadeOut(DefaultDecal.DecalLifeTime, DefaultDecal.DecalFadeOutTime);
			}
		
			if (IsDebugEnabled)
			{
				DrawDebugPoint(GetWorld(), ShotEnd, 10.f, FColor::Red, false, 5.f);
			}
		}
	
		UNiagaraComponent* TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, MuzzleLocation, GetComponentRotation());
		TraceFXComponent->SetVectorParameter(NiagaraProperty::TraceEnd, ShotEnd);
	
		if (IsDebugEnabled)
		{
			DrawDebugLine(GetWorld(), MuzzleLocation, ShotEnd, FColor::Red, false, 5.f, 0.f, 1.f);
		}
	}
}

FVector UWeaponBarrelComponent::GetBulletSpreadOffset(const float Angle, const FRotator ShotRotation) const
{
	const float SpreadSize = FMath::Tan(Angle);
	const float RotationAngle = FMath::RandRange(0.f, 2 * PI);
	const float SpreadY = FMath::Cos(RotationAngle);
	const float SpreadZ = FMath::Sin(RotationAngle);

	return (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ + ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;
}

float UWeaponBarrelComponent::GetCurrentFireRange() const
{
	return bIsAiming ? AimingFireRange : FireRange;
}

float UWeaponBarrelComponent::GetCurrentDamage() const
{
	return bIsAiming ? AimingDamage : BaseDamage;
}
