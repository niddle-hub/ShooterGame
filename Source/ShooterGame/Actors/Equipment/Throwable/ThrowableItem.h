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

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ThrowableItem")
	TSubclassOf<class ASGProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ThrowableItem", meta = (UIMin = -90.f, UIMax = 90.f))
	float ThrowAngle = 0.f;
};
