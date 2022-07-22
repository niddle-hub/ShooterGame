#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponAmmoWidget.generated.h"

UCLASS()
class SHOOTERGAME_API UWeaponAmmoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ammo")
	int32 Ammo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Ammo")
	int32 TotalAmmo;

private:
	UFUNCTION()
	void UpdateAmmo(int32 NewAmmo, int32 NewTotalAmmo);
};
