#include "PlayerHUDWidget.h"
#include "ThrowableItemWidget.h"
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

UThrowableItemWidget* UPlayerHUDWidget::GetThrowableItemWidget() const
{
	return WidgetTree->FindWidget<UThrowableItemWidget>(ThrowableItemWidgetName);
}

UCharacterAttributesWidget* UPlayerHUDWidget::GetCharacterAttributesWidget() const
{
	return WidgetTree->FindWidget<UCharacterAttributesWidget>(CharacterAttributesWidgetName);
}
