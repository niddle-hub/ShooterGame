#pragma once

#define ECC_Climbing ECC_GameTraceChannel1
#define ECC_InteractionVolume ECC_GameTraceChannel2

namespace Debug
{
	static const FName LedgeDetection = "LedgeDetection";
	static const FName CharacterAttributes = "CharacterAttributes";
}

namespace CollisionProfile
{
	static const FName Pawn = "Pawn";
	static const FName InteractOnlyPawn = "InteractOnlyPawn";
	static const FName Ragdoll = "Ragdoll";
}

namespace Socket
{
	const FName FPCamera = "CameraSocket";
}
