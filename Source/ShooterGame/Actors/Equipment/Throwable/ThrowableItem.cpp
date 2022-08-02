#include "ThrowableItem.h"
#include "ShooterGame/Actors/Projectile/SGProjectile.h"
#include "ShooterGame/Characters/BaseCharacter.h"

void AThrowableItem::Throw() const
{
	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("ThrowableItem must be owned by a BaseCharacter"));
	ABaseCharacter* CharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());
	
	const APlayerController* PlayerController = CharacterOwner->GetController<APlayerController>();
	if (!IsValid(PlayerController))
	{
		return;
	}
	
	FVector ViewPoint;
	FRotator ViewRotation;
	PlayerController->GetPlayerViewPoint(ViewPoint, ViewRotation);

	const FTransform ViewTransform = FTransform(ViewRotation, ViewPoint);
	const FVector ViewDirection = ViewRotation.RotateVector(FVector::ForwardVector);
	const FVector ViewUpVector = ViewRotation.RotateVector(FVector::UpVector);
	const FVector LaunchDirection = ViewDirection + FMath::Tan(FMath::DegreesToRadians(ThrowAngle)) * ViewUpVector;
	const FVector ThrowableLocation = CharacterOwner->GetMesh()->GetSocketLocation(Socket::Throwable_R);
	const FVector SocketInViewSpace = ViewTransform.InverseTransformPosition(ThrowableLocation);
	const FVector SpawnLocation = ViewPoint + ViewDirection * SocketInViewSpace.X;
	
	ASGProjectile* Projectile = GetWorld()->SpawnActor<ASGProjectile>(ProjectileClass, SpawnLocation, FRotator::ZeroRotator);
	if (IsValid(Projectile))
	{
		Projectile->SetOwner(CharacterOwner);
		Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
	}
}


