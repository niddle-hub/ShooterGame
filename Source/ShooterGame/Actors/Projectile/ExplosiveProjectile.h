#pragma once

#include "CoreMinimal.h"
#include "SGProjectile.h"
#include "ExplosiveProjectile.generated.h"

UCLASS()
class SHOOTERGAME_API AExplosiveProjectile : public ASGProjectile
{
	GENERATED_BODY()

public:
	AExplosiveProjectile();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UExplosionComponent* ExplosionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Explosion")
	float DetonationDelay = 2.f;

	virtual void OnProjectileLaunched() override;

private:
	FTimerHandle DetonationTimerHandle;
	AController* GetController() const;
	void OnDetonationTimerElapsed() const;
};
