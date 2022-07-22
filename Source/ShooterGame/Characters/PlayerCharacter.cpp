#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ShooterGame/Actors/Equipment/Weapons/RangeWeaponItem.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetupAttachment(RootComponent);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->bUsePawnControlRotation = false;
	CameraComponent->SetupAttachment(SpringArmComponent);

	GetCharacterMovement()->bOrientRotationToMovement = 1;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpringArmComponent->TargetArmLength = DefaultSpringArmLength;
	
	if (IsValid(SprintTimelineCurve))
	{
		FOnTimelineFloatStatic SpringArmTimelineUpdate;
		SpringArmTimelineUpdate.BindUObject(this, &APlayerCharacter::SpringArmSprintTimelineUpdate);
		SpringArmSprintTimeline.AddInterpFloat(SprintTimelineCurve, SpringArmTimelineUpdate);
	}

	if (IsValid(AimingTimelineCurve))
	{
		FOnTimelineFloatStatic AimingTimelineUpdate;
		AimingTimelineUpdate.BindUObject(this, &APlayerCharacter::AimingFOVTimelineUpdate);
		AimingFOVTimeline.AddInterpFloat(AimingTimelineCurve, AimingTimelineUpdate);
	}
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	SpringArmSprintTimeline.TickTimeline(DeltaSeconds);
	AimingFOVTimeline.TickTimeline(DeltaSeconds);
	
	if (!AreRequiredKeysDown())
	{
		BaseCharacterMovementComponent->EndSprint();
		OnSprintEnd_Implementation();
	}
}

void APlayerCharacter::MoveForward(float Value)
{
	ForwardAxis = Value;
	if ((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		const FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		const FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	RightAxis = Value;
	if ((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		const FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		const FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value * GetAimTurnModifier());
}

void APlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value * GetAimLookUpModifier());
}

void APlayerCharacter::TurnAtRate(float Value)
{
	AddControllerYawInput(Value * GetAimTurnModifier() * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Value)
{
	AddControllerPitchInput(Value * GetAimLookUpModifier() * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::ChangeCrouchState()
{
	if (BaseCharacterMovementComponent->IsFalling())
	{
		return;
	}
	
	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void APlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset.Z += HalfHeightAdjust;
}

void APlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset.Z -= HalfHeightAdjust;
}

bool APlayerCharacter::CanJumpInternal_Implementation() const
{
	//HomeWork "Mantle BugFix" 3.Error when mantling from a crouch state - Start.
	return (bIsCrouched || Super::CanJumpInternal_Implementation()) && !GetBaseCharacterMovementComponent()->IsMantling();
	//HomeWork "Mantle BugFix" 3.Error when mantling from a crouch state - End.
}

void APlayerCharacter::OnJumped_Implementation()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

bool APlayerCharacter::CanSprint()
{
	return Super::CanSprint() && AreRequiredKeysDown();
}

void APlayerCharacter::OnSprintStart_Implementation()
{
	SpringArmSprintTimeline.Play();
	StopAiming();
}

void APlayerCharacter::OnSprintEnd_Implementation()
{
	SpringArmSprintTimeline.Reverse();
}

void APlayerCharacter::SwimForward(float Value)
{
	ForwardAxis = Value;
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		const FRotator PitchYawRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0.f);
		const FVector ForwardVector = PitchYawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::SwimRight(float Value)
{
	RightAxis = Value;
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		const FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		const FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::SwimUp(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		AddMovementInput(FVector::UpVector, Value);
	}
}

void APlayerCharacter::OnStartAimingInternal()
{
	Super::OnStartAimingInternal();

	if (!IsValid(AimingTimelineCurve))
	{
		const APlayerController* PlayerController = GetController<APlayerController>();
		if (!IsValid(PlayerController))
		{
			return;
		}

		APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager;
		if (IsValid(PlayerCameraManager))
		{
			const ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetEquippedRangeWeapon();
			PlayerCameraManager->SetFOV(CurrentRangeWeapon->GetAimFOV());
		}
	}
	else
	{
		AimingFOVTimeline.Play();
	}
}

void APlayerCharacter::OnStopAimingInternal()
{
	Super::OnStopAimingInternal();
	
	if (!IsValid(AimingTimelineCurve))
	{
		const APlayerController* PlayerController = GetController<APlayerController>();
		if (!IsValid(PlayerController))
		{
			return;
		}

		APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager;
		if (IsValid(PlayerCameraManager))
		{
			PlayerCameraManager->UnlockFOV();
		}
	}
	else
	{
		AimingFOVTimeline.Reverse();
	}
}

void APlayerCharacter::SpringArmSprintTimelineUpdate(const float Alpha) const
{
	const float SpringArmTargetLength = FMath::Lerp(DefaultSpringArmLength, TargetSpringArmLength, Alpha);
	SpringArmComponent->TargetArmLength = SpringArmTargetLength;
}

void APlayerCharacter::AimingFOVTimelineUpdate(const float Alpha) const
{
	const APlayerController* PlayerController = GetController<APlayerController>();
	if (!IsValid(PlayerController))
	{
		return;
	}
	const ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetEquippedRangeWeapon();
	const float TargetFOV = CurrentRangeWeapon->GetAimFOV();
	APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager;
	const float DefaultFOV = PlayerCameraManager->DefaultFOV;
	
	const float FOV = FMath::Lerp(DefaultFOV, TargetFOV, Alpha);
	
	if (IsValid(PlayerCameraManager))
	{
		PlayerCameraManager->SetFOV(FOV);
	}
}

float APlayerCharacter::GetAimTurnModifier() const
{
	const float TurnModifier = CharacterEquipmentComponent->GetEquippedRangeWeapon()->GetAimTurnModifier();
	return IsAiming() ? TurnModifier : 1.f;
}

float APlayerCharacter::GetAimLookUpModifier() const
{
	const float LookUpModifier = CharacterEquipmentComponent->GetEquippedRangeWeapon()->GetAimLookUpModifier();
	return IsAiming() ? LookUpModifier : 1.f;
}

bool APlayerCharacter::AreRequiredKeysDown() const
{
	if (ForwardAxis < 0.1f || !FMath::IsNearlyZero(RightAxis))
	{
		return false;
	}

	return true;
}
