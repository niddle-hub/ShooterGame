#include "ExplosiveProjectile.h"
#include "ShooterGame/Components/WeaponComponents/ExplosionComponent.h"

AExplosiveProjectile::AExplosiveProjectile()
{
	ExplosionComponent = CreateDefaultSubobject<UExplosionComponent>(TEXT("ExplosionComponent"));
	ExplosionComponent->SetupAttachment(GetRootComponent());
}

void AExplosiveProjectile::OnProjectileLaunched()
{
	Super::OnProjectileLaunched();
	GetWorldTimerManager().SetTimer(DetonationTimerHandle, this, &AExplosiveProjectile::OnDetonationTimerElapsed, DetonationDelay, false);
}

AController* AExplosiveProjectile::GetController() const
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	return IsValid(Pawn) ? Pawn->GetController() : nullptr;
}

void AExplosiveProjectile::OnDetonationTimerElapsed() const
{
	ExplosionComponent->Explode(GetController());
}
