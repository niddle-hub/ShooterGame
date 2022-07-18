#pragma once

#define ECC_Climbing ECC_GameTraceChannel1
#define ECC_InteractionVolume ECC_GameTraceChannel2

namespace Debug
{
	static const FName LedgeDetection = "LedgeDetection";
}

const FName SocketFPCamera = FName("CameraSocket");

static const FName OverlapCollisionProfilePawn = "Pawn";
static const FName OverlapCollisionProfilePawnInteraction = "InteractOnlyPawn";
