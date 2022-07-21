// ReSharper disable UnrealHeaderToolParserError
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WeaponBarrelComponent.generated.h"

class UNiagaraSystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERGAME_API UWeaponBarrelComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	void Shot(FVector ShotStart, FVector ShotDirection, AController* Controller) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel settings|Fire")
	float FireRange = 10000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel settings|Damage")
	float BaseDamage = 15.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel settings|Damage")
	UCurveFloat* DamageCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel settings|VFX")
	UNiagaraSystem* FireFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel settings|VFX")
	UNiagaraSystem* TraceFX;
};
