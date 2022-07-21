#pragma once

#define ECC_Climbing ECC_GameTraceChannel1
#define ECC_InteractionVolume ECC_GameTraceChannel2
#define ECC_Bullet ECC_GameTraceChannel3

namespace Debug
{
	static const FName LedgeDetection = "LedgeDetection";
	static const FName CharacterAttributes = "CharacterAttributes";
	static const FName RangeWeapon = "RangeWeapon";
}

namespace CollisionProfile
{
	static const FName Pawn = "Pawn";
	static const FName InteractOnlyPawn = "InteractOnlyPawn";
	static const FName Ragdoll = "Ragdoll";
}

namespace Socket
{
	static const FName FPCamera = "CameraSocket";
	static const FName Weapon_R = "WeaponSocket_R";
	static const FName Muzzle = "Muzzle_Socket";
}

namespace NiagaraProperty
{
	static const FName TraceEnd = "TraceEnd";
}

UENUM(BlueprintType)
enum class EEquipableItemType : uint8
{
	EIT_None UMETA(DisplayName = "None"),
	EIT_Pistol UMETA(DisplayName = "Pistol"),
	EIT_MAX UMETA(Hidden)
};
