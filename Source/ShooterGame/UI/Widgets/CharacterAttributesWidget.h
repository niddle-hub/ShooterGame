#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterAttributesWidget.generated.h"

UCLASS()
class SHOOTERGAME_API UCharacterAttributesWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnStaminaChanged(float NewStamina);

	UFUNCTION(BlueprintImplementableEvent)
	void OnOxygenChanged(float NewOxygen);

	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float NewHealth);
	
	UFUNCTION(BlueprintCallable)
	float GetOxygen() const;

	UFUNCTION(BlueprintCallable)
	float GetOxygenPercent() const;
	
	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable)
	float GetHealthPercent() const;

	UFUNCTION(BlueprintCallable)
	float GetStamina() const;

	UFUNCTION(BlueprintCallable)
	float GetStaminaPercent() const;

private:
	TWeakObjectPtr<class ABaseCharacter> CharacterOwner;

	UFUNCTION()
	void UpdateHealth(float NewHealth);
	UFUNCTION()
	void UpdateOxygen(float NewOxygen);
	UFUNCTION()
	void UpdateStamina(float NewStamina);
};
