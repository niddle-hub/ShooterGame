#include "BaseCharacterAnimInstance.h"

void UBaseCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<ABaseCharacter>(), TEXT("BaseCharacterAnimInstance can work only with ABaseCharacter instance class"));
	CachedCharacter = StaticCast<ABaseCharacter*>(TryGetPawnOwner());
}

void UBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CachedCharacter.IsValid())
	{
		return;
	}

	const UBaseCharacterMovementComponent* CharacterMovementComponent = CachedCharacter->GetBaseCharacterMovementComponent();
	Speed = CharacterMovementComponent->Velocity.Size();
	WalkSpeed = CharacterMovementComponent->MaxWalkSpeed;
	IsFalling = CharacterMovementComponent->IsFalling();
	IsCrouching = CharacterMovementComponent->IsCrouching();
	IsSprinting = CharacterMovementComponent->IsSprinting();
	IsOutOfStamina = CharacterMovementComponent->GetStaminaIsOver();
	IsSwimming = CharacterMovementComponent->IsSwimming();
	IsOnLadder = CharacterMovementComponent->IsOnLadder();
	if (IsOnLadder)
	{
		LadderSpeedRatio = CharacterMovementComponent->GetLadderSpeedRatio();
	}

	IsStrafing = !CharacterMovementComponent->bOrientRotationToMovement;
	Direction = CalculateDirection(CharacterMovementComponent->Velocity, CachedCharacter->GetActorRotation());
	
	RightFootEffectorLocation = FVector(CachedCharacter->GetIKRightFootOffset() + CachedCharacter->GetIKPelvisOffset(), 0.f, 0.f);
	LeftFootEffectorLocation = FVector(-(CachedCharacter->GetIKLeftFootOffset() + CachedCharacter->GetIKPelvisOffset()), 0.f, 0.f);
	PelvisBoneTransitionOffset = FVector(0.f, 0.f, CachedCharacter->GetIKPelvisOffset());

	const UCharacterEquipmentComponent* EquipmentComponent = CachedCharacter->GetCharacterEquipmentComponent();
	EquippedItemType = EquipmentComponent->GetEquippedItemType();

	AimingRotation = CachedCharacter->GetBaseAimRotation();
}
