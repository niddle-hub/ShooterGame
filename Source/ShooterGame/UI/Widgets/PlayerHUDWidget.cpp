#include "PlayerHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "ShooterGame/Characters/BaseCharacter.h"
#include "ShooterGame/UI/Widgets/ReticleWidget.h"
#include "ShooterGame/UI/Widgets/WeaponAmmoWidget.h"
#include "ShooterGame/Components/CharacterComponents/CharacterAttributesComponent.h"


UReticleWidget* UPlayerHUDWidget::GetReticleWidget() const
{
	return WidgetTree->FindWidget<UReticleWidget>(ReticleWidgetName);
}

UWeaponAmmoWidget* UPlayerHUDWidget::GetWeaponAmmoWidget() const
{
	return WidgetTree->FindWidget<UWeaponAmmoWidget>(WeaponAmmoWidgetName);
}

float UPlayerHUDWidget::GetHealth() const
{
	float Result = 1.f;
	APawn* Pawn = GetOwningPlayerPawn();
	const ABaseCharacter* Character = Cast<ABaseCharacter>(Pawn);
	if (IsValid(Character))
	{
		Result = Character->GetCharacterAttributesComponent()->GetHealth();
	}

	return Result;
}

float UPlayerHUDWidget::GetHealthPercent() const
{
	float Result = 1.f;
	APawn* Pawn = GetOwningPlayerPawn();
	const ABaseCharacter* Character = Cast<ABaseCharacter>(Pawn);
	if (IsValid(Character))
	{
		Result = Character->GetCharacterAttributesComponent()->GetHealthPercent();
	}

	return Result;
}
