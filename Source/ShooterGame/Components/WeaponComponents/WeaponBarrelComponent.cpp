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

void UWeaponBarrelComponent::Shot(const FVector ShotStart, const FVector ShotDirection, AController* Controller) const
{
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
			HitActor->TakeDamage(DamageAmount, FDamageEvent(), Controller, GetOwner());
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

float UWeaponBarrelComponent::GetCurrentFireRange() const
{
	return bIsAiming ? AimingFireRange : FireRange;
}

float UWeaponBarrelComponent::GetCurrentDamage() const
{
	return bIsAiming ? AimingDamage : BaseDamage;
}
