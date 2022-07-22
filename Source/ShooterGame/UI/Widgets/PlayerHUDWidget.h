#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

UCLASS()
class SHOOTERGAME_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	class UReticleWidget* GetReticleWidget() const;

	class UWeaponAmmoWidget* GetWeaponAmmoWidget() const;

protected:
	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable)
	float GetHealthPercent() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle")
	FName ReticleWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
	FName WeaponAmmoWidgetName;
};
