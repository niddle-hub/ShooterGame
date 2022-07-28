#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ExplosionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExposionSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERGAME_API UExplosionComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Explode(AController* InstigatorController);

	UPROPERTY(BlueprintAssignable)
	FOnExposionSignature OnExplosionDelegate;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Damage")
	float MaxDamage = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Damage")
	float MinDamage = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Damage")
	float DamageFallOff = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Damage")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Radius")
	float InnerRadius = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|Radius")
	float OuterRadius = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion|VFX")
	UParticleSystem* ExplosionVFX;

private:
	bool bIsExploding = false;
};
