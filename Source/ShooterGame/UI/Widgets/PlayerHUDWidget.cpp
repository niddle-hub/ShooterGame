#include "PlayerHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "ShooterGame/UI/Widgets/ReticleWidget.h"
#include "ShooterGame/UI/Widgets/WeaponAmmoWidget.h"
#include "ShooterGame/UI/Widgets/CharacterAttributesWidget.h"


UReticleWidget* UPlayerHUDWidget::GetReticleWidget() const
{
	return WidgetTree->FindWidget<UReticleWidget>(ReticleWidgetName);
}

UWeaponAmmoWidget* UPlayerHUDWidget::GetWeaponAmmoWidget() const
{
	return WidgetTree->FindWidget<UWeaponAmmoWidget>(WeaponAmmoWidgetName);
}

UCharacterAttributesWidget* UPlayerHUDWidget::GetCharacterAttributesWidget() const
{
	return WidgetTree->FindWidget<UCharacterAttributesWidget>(CharacterAttributesWidgetName);
}
