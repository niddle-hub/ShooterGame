#include "ExplosionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

void UExplosionComponent::Explode(AController* InstigatorController)
{
	if (bIsExploding)
	{
		return;
	}
	bIsExploding = true;
	
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(GetOwner());\
	
	UGameplayStatics::ApplyRadialDamageWithFalloff(
		GetWorld(),
		MaxDamage,
		MinDamage,
		GetComponentLocation(),
		InnerRadius,
		OuterRadius,
		DamageFallOff,
		DamageType,
		IgnoredActors,
		GetOwner(),
		InstigatorController,
		ECC_Visibility);
	
	if (IsValid(ExplosionVFX))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetComponentLocation());
	}
	
	if (OnExplosionDelegate.IsBound())
	{
		OnExplosionDelegate.Broadcast();
	}
}
