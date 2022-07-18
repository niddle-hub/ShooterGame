#include "BasePawnAnimInstance.h"
#include "../../Components/MovementComponents/GameCodePawnMovementComponent.h"
#include "ShooterGame/Characters/GameCodeBasePawn.h"

void UBasePawnAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AGameCodeBasePawn>(),
	       TEXT("UBasePawnAnimInstance::NativeBeginPlay() can work only with AGameCodeBasePawn class"));
	CachedPawn = StaticCast<AGameCodeBasePawn*>(TryGetPawnOwner());
}

void UBasePawnAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CachedPawn.IsValid())
	{
		return;
	}

	InputForward = CachedPawn->GetInputForward();
	InputRight = CachedPawn->GetInputRight();
	IsInAir = CachedPawn->GetMovementComponent()->IsFalling();
}
