#pragma once

#include "CoreMinimal.h"
#include "ShooterGame/Actors/Equipment/EquipableItem.h"
#include "ThrowableItem.generated.h"

UCLASS(Blueprintable)
class SHOOTERGAME_API AThrowableItem : public AEquipableItem
{
	GENERATED_BODY()

public:
	void Throw() const;

	bool IsUnlimited() const { return bIsUnlimitedUsage; }

	EAmmunitionType GetAmmunitionType() const { return AmmunitionType; };

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ThrowableItem")
	TSubclassOf<class ASGProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ThrowableItem", meta = (UIMin = -90.f, UIMax = 90.f))
	float ThrowAngle = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ThrowableItem")
	EAmmunitionType AmmunitionType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ThrowableItem")
	bool bIsUnlimitedUsage = false;
};
