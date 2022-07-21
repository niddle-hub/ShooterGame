#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShooterGame/ShooterGameTypes.h"
#include "CharacterEquipmentComponent.generated.h"

class ARangeWeaponItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERGAME_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	EEquipableItemType GetEquippedItemType() const;

	FORCEINLINE ARangeWeaponItem* GetEquippedRangeWeapon() const { return EquippedWeapon; }

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<ARangeWeaponItem> RangeWeaponItemClass;

private:
	void CreateLoadout();
	ARangeWeaponItem* EquippedWeapon;

	TWeakObjectPtr<class ABaseCharacter> OwnerCharacter;
		
};
