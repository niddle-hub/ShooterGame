#include "CharacterAttributesWidget.h"
#include "ShooterGame/Characters/BaseCharacter.h"
#include "ShooterGame/Components/CharacterComponents/CharacterAttributesComponent.h"

void UCharacterAttributesWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	APawn* Pawn = GetOwningPlayerPawn();
	ABaseCharacter* Character = Cast<ABaseCharacter>(Pawn);
	if (IsValid(Character))
	{
		CharacterOwner = Character;
	}
}

float UCharacterAttributesWidget::GetOxygen() const
{
	float Result = 1.f;
	if (CharacterOwner.IsValid())
	{
		Result = CharacterOwner->GetCharacterAttributesComponent()->GetOxygen();
	}

	return Result;
}

float UCharacterAttributesWidget::GetOxygenPercent() const
{
	float Result = 1.f;
	if (CharacterOwner.IsValid())
	{
		Result = CharacterOwner->GetCharacterAttributesComponent()->GetOxygenPercent();
	}

	return Result;
}

float UCharacterAttributesWidget::GetHealth() const
{
	float Result = 1.f;
	if (CharacterOwner.IsValid())
	{
		Result = CharacterOwner->GetCharacterAttributesComponent()->GetHealth();
	}

	return Result;
}

float UCharacterAttributesWidget::GetHealthPercent() const
{
	float Result = 1.f;
	if (CharacterOwner.IsValid())
	{
		Result = CharacterOwner->GetCharacterAttributesComponent()->GetHealthPercent();
	}
	return Result;
}

float UCharacterAttributesWidget::GetStamina() const
{
	float Result = 1.f;
	if (CharacterOwner.IsValid())
	{
		Result = CharacterOwner->GetCharacterAttributesComponent()->GetStamina();
	}

	return Result;
}

float UCharacterAttributesWidget::GetStaminaPercent() const
{
	float Result = 1.f;
	if (CharacterOwner.IsValid())
	{
		Result = CharacterOwner->GetCharacterAttributesComponent()->GetStaminaPercent();
	}

	return Result;
}

void UCharacterAttributesWidget::UpdateHealth(float NewHealth)
{
	OnHealthChanged(NewHealth);
}

void UCharacterAttributesWidget::UpdateOxygen(float NewOxygen)
{
	OnOxygenChanged(NewOxygen);
}

void UCharacterAttributesWidget::UpdateStamina(float NewStamina)
{
	OnStaminaChanged(NewStamina);
}
