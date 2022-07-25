#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShooterGame/Characters/BaseCharacter.h"
#include "CharacterAttributesComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeathSignature)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStaminaChangedSignature, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOxygeChangedSignature, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FOutOfStaminaSignature, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOutOfOxygenSignature, bool)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERGAME_API UCharacterAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterAttributesComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	FOnDeathSignature OnDeathEvent;
	FOnStaminaChangedSignature OnStaminaChangedEvent;
	FOnHealthChangedSignature OnHealthChangedEvent;
	FOnOxygeChangedSignature OnOxygenChangedEvent;
	FOutOfStaminaSignature OnOutOfStaminaEvent;
	FOutOfOxygenSignature OnOutOfOxygenEvent;

	bool IsAlive() const { return CurrentHealth > 0.f; }

	float GetHealth() const { return CurrentHealth; }

	float GetMaxHealth() const { return MaxHealth; }

	float GetHealthPercent() const { return CurrentHealth / MaxHealth; }

	float GetOxygen() const { return CurrentOxygen; }

	float GetMaxOxygen() const { return MaxOxygen; }

	float GetOxygenPercent() const { return CurrentOxygen / MaxOxygen; }

	float GetStamina() const { return CurrentStamina; }

	float GetMaxStamina() const { return MaxStamina; }

	float GetStaminaPercent() const { return CurrentStamina / MaxStamina; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Health", meta=(UIMin = 0))
	float MaxHealth = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Sprint", meta=(UIMin = 0))
	float MaxStamina = 100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Oxygen", meta=(UIMin = 0))
	float MaxOxygen = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Sprint")
	float StaminaRestoreVelocity = 30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Sprint")
	float StaminaConsumptionVelocity = 10.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Oxygen")
	float OxygenRestoreVelocity = 15.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Oxygen")
	float OxygenConsumptionVelocity = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Oxygen")
	float OxygenDamage = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes|Oxygen")
	float OxygenDamageDelay = 2.f;

private:
	void SetHealth(float NewHealth);
	void SetStamina(float NewStamina);
	void SetOxygen(float NewOxygen);
	
	float CurrentHealth = 0.f;
	float CurrentStamina = 0.f;
	float CurrentOxygen = 0.f;

	static bool IsValidDamage(const float Damage) { return Damage > 0.f; }
	void UpdateStamina(float DeltaTime);

	FTimerHandle OxygenDamageTimerHandle;
	void UpdateOxygen(float DeltaTime);
	void OnOxygenHasChanged(bool InState);
	void TakeOxygenDamage() const;

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	void DrawDebugAttributes() const;
#endif

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser );

	TWeakObjectPtr<ABaseCharacter> CachedBaseCharacterOwner;
	
};
