#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterGame/ShooterGameTypes.h"
#include "EquipableItem.generated.h"

UCLASS(Abstract, NotBlueprintable)
class SHOOTERGAME_API AEquipableItem : public AActor
{
	GENERATED_BODY()
public:
	EEquipableItemType GetItemType() const { return ItemType; }
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipableItem")
	EEquipableItemType ItemType = EEquipableItemType::EIT_None;
};
