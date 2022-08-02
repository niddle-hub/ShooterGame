#include "EquipableItem.h"

void AEquipableItem::Equip()
{
	if (OnEquipmentStateChangedDelegate.IsBound())
	{
		OnEquipmentStateChangedDelegate.Broadcast(true);
	}
}

void AEquipableItem::Unequip()
{
	if (OnEquipmentStateChangedDelegate.IsBound())
	{
		OnEquipmentStateChangedDelegate.Broadcast(false);
	}
}
