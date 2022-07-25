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

	FName GetUnequippedSocketName() const { return UnequippedSocketName; }

	FName GetEquippedSocketName() const { return EquippedSocketName; }

	UAnimMontage* GetCharacterEquipAnimation() const { return EquipAnimation; }
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipableItem")
	EEquipableItemType ItemType = EEquipableItemType::EIT_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipableItem")
	UAnimMontage* EquipAnimation = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipableItem")
	FName UnequippedSocketName =  NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipableItem")
	FName EquippedSocketName =  NAME_None;
};
