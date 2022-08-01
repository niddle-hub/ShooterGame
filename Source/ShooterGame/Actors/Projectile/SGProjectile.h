#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGProjectile.generated.h"

UCLASS()
class SHOOTERGAME_API ASGProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ASGProjectile();

	UFUNCTION(BlueprintCallable)
	void LaunchProjectile(FVector Direction);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;
};
