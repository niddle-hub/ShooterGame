#include "BaseCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "GameFramework/Character.h"
#include "ShooterGame/Actors/Interactive/Environment/Ladder.h"
#include "ShooterGame/Actors/Interactive/Environment/Zipline.h"
#include "ShooterGame/Characters/BaseCharacter.h"

void UBaseCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	if (ForceRotation)
	{
		const FRotator CurrentRotation = UpdatedComponent->GetComponentRotation();
		CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

		const FRotator DeltaRot = GetDeltaRotation(DeltaTime);
		DeltaRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): GetDeltaRotation"));

		constexpr float AngleTolerance = 1e-3f;

		if (!CurrentRotation.Equals(ForceTargetRotation, AngleTolerance))
		{
			FRotator DesiredRotation = ForceTargetRotation;
			// PITCH
			if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
			{
				DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
			}

			// YAW
			if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
			{
				DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
			}

			// ROLL
			if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
			{
				DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
			}

			// Set the new rotation.
			DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
			MoveUpdatedComponent( FVector::ZeroVector, DesiredRotation, /*bSweep*/ false );
		}
		else
		{
			ForceTargetRotation = FRotator::ZeroRotator;
			ForceRotation = false;
		}
		return;
	}
	
	if (IsOnLadder() || IsOnZipline())
	{
		return;
	}
	
	Super::PhysicsRotation(DeltaTime);
}

float UBaseCharacterMovementComponent::GetMaxSpeed() const
{
	float Result = Super::GetMaxSpeed();
	if (bIsSprinting)
	{
		Result = SprintSpeed;
	}
	if (bStaminaIsOver)
	{
		Result = RestWalkSpeed;
	}
	if (IsOnLadder())
	{
		Result = ClimbingOnLadderSpeed;
	}
	if (IsOnZipline())
	{
		Result = ZiplineMovementSpeed;
	}
	return Result;
}

void UBaseCharacterMovementComponent::StartSprint()
{
	bIsSprinting = true;
	bForceMaxAccel = 1;
}

void UBaseCharacterMovementComponent::EndSprint()
{
	bIsSprinting = false;
	bForceMaxAccel = 0;
}

void UBaseCharacterMovementComponent::StartMantle(const FMantlingMovementParameters& MantlingMovementParameters)
{
	CurrentMantlingParameters = MantlingMovementParameters;
	SetMovementMode(MOVE_Custom, static_cast<uint8>(ECustomMovementMode::CMOVE_Mantling));
}

void UBaseCharacterMovementComponent::EndMantle()
{
	SetMovementMode(MOVE_Walking);
}

bool UBaseCharacterMovementComponent::IsMantling() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == static_cast<uint8>(ECustomMovementMode::CMOVE_Mantling);
}

float UBaseCharacterMovementComponent::GetActorToLadderProjection(const FVector& ActorLocation) const
{
	checkf(IsValid(CurrentLadder), TEXT("UBaseCharacterMovementComponent::GetCharacterToLadderProjection() can't be invoked when CurrentLadder is invalid"));
	
	const FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	const FVector DistanceToLadder = ActorLocation - CurrentLadder->GetActorLocation();
	return FVector::DotProduct(LadderUpVector, DistanceToLadder);
}

float UBaseCharacterMovementComponent::GetLadderSpeedRatio() const
{
	checkf(IsValid(CurrentLadder), TEXT("UBaseCharacterMovementComponent::GetCharacterToLadderProjection() can't be invoked when CurrentLadder is invalid"));
	
	const FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	return FVector::DotProduct(LadderUpVector, Velocity) / ClimbingOnLadderSpeed;
}

void UBaseCharacterMovementComponent::AttachToLadder(const ALadder* Ladder)
{
	CurrentLadder = Ladder;
	
	FRotator TargetOrientation = CurrentLadder->GetActorForwardVector().ToOrientationRotator();
	TargetOrientation.Yaw += 180.f;

	const FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	const FVector LadderForwardVector = CurrentLadder->GetActorForwardVector();
	const float ProjectionToLadder = GetActorToLadderProjection(GetActorLocation());

	FVector TargetLocation = CurrentLadder->GetActorLocation() + ProjectionToLadder * LadderUpVector + CharacterToLadderOffset * LadderForwardVector;
	
	if (CurrentLadder->IsOnTop())
	{
		TargetLocation = CurrentLadder->GetAttachFromTopAnimMontageStartLocation();
	}

	GetOwner()->SetActorLocation(TargetLocation);
	GetOwner()->SetActorRotation(TargetOrientation);
	
	SetMovementMode(MOVE_Custom, static_cast<uint8>(ECustomMovementMode::CMOVE_OnLadder));
}

void UBaseCharacterMovementComponent::DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod)
{
	switch (DetachFromLadderMethod)
	{
		case EDetachFromLadderMethod::DLM_ReachingTheTop:
		{
			GetBaseCharacterOwner()->Mantle(true);
			break;
		}
		case EDetachFromLadderMethod::DLM_ReachingTheButtom:
		{
			SetMovementMode(MOVE_Walking);
			break;
		}
		case EDetachFromLadderMethod::DLM_JumpOff:
		{
			const FVector JumpDirection = CurrentLadder->GetActorForwardVector();
			const FVector JumpVelocity = JumpDirection * JumpOffLadderSpeed;
				
			SetMovementMode(MOVE_Falling);
			ForceTargetRotation = JumpDirection.ToOrientationRotator();
			ForceRotation = true;
			Launch(JumpVelocity);
			break;
		}
		default:
		{
			SetMovementMode(MOVE_Falling);
			break;
		}
	}
}

bool UBaseCharacterMovementComponent::IsOnLadder() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == static_cast<uint8>(ECustomMovementMode::CMOVE_OnLadder);
}

void UBaseCharacterMovementComponent::AttachToZipline(const AZipline* Zipline)
{
	CurrentZipline = Zipline;
	const FRotator TargetOrientation = CurrentZipline->GetActorForwardVector().ToOrientationRotator();
	GetOwner()->SetActorRotation(TargetOrientation);
	
	SetMovementMode(MOVE_Custom, static_cast<uint8>(ECustomMovementMode::CMOVE_OnZipline));
}

void UBaseCharacterMovementComponent::DetachFromZipline(EDetachFromZiplineMethod DetachFromZiplineMethod)
{
	switch (DetachFromZiplineMethod)
	{
		case EDetachFromZiplineMethod::DZM_ReachingTheEnd:
		{
			SetMovementMode(MOVE_Falling);
			break;
		}
		case  EDetachFromZiplineMethod::DZM_JumpOff:
		{
			const FVector JumpDirection = CurrentZipline->GetActorForwardVector();
			const FVector JumpVelocity = JumpDirection * JumpOffZiplineSpeed;

			SetMovementMode(MOVE_Falling);
			ForceTargetRotation = JumpDirection.ToOrientationRotator();
			ForceRotation = true;
			Launch(JumpVelocity);
			break;
		}
		default:
		{
			SetMovementMode(MOVE_Falling);
			break;
		}
	}
}

bool UBaseCharacterMovementComponent::IsOnZipline() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == static_cast<uint8>(ECustomMovementMode::CMOVE_OnZipline);
}

void UBaseCharacterMovementComponent::PhysMantling(float, int32)
{
	const float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(MantlingTimer) + CurrentMantlingParameters.StartTime;

	const FVector MantlingCurveValue = CurrentMantlingParameters.MantlingCurve->GetVectorValue(ElapsedTime);
	const FVector TargetLocation = CurrentMantlingParameters.GetWorldTargetLocation();
				
	const float PositionAlpha = MantlingCurveValue.X;
	const float XYCorrectionAlpha = MantlingCurveValue.Y;
	const float ZCorrectionAlpha = MantlingCurveValue.Z;

	FVector CorrectedInitialLocation = FMath::Lerp(CurrentMantlingParameters.InitialLocation, CurrentMantlingParameters.InitialAnimationLocation, XYCorrectionAlpha);
	CorrectedInitialLocation.Z = FMath::Lerp(CurrentMantlingParameters.InitialLocation.Z, CurrentMantlingParameters.InitialAnimationLocation.Z, ZCorrectionAlpha);

	const FVector NewLocation = FMath::Lerp(CorrectedInitialLocation, TargetLocation, PositionAlpha);
	const FRotator NewRotation = FMath::Lerp(CurrentMantlingParameters.InitialRotation, CurrentMantlingParameters.TargetRotation, PositionAlpha);

	const FVector Delta = NewLocation - GetActorLocation();
				
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, NewRotation, false, Hit);
}

void UBaseCharacterMovementComponent::PhysLadder(float DeltaTime, int32 Iterations)
{
	CalcVelocity(DeltaTime, 1.f, false, ClimbingOnLadderBrakingDeceleration);
	const FVector Delta = Velocity * DeltaTime;

	if (HasAnimRootMotion())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, Hit);
		return;
	}

	const FVector NewLocation = GetActorLocation() + Delta;
	const float NewLocationProjection = GetActorToLadderProjection(NewLocation);

	if (NewLocationProjection < MinLadderBottomOffset)
	{
		DetachFromLadder(EDetachFromLadderMethod::DLM_ReachingTheButtom);
		return;
	}
	
	if (NewLocationProjection > (CurrentLadder->GetLadderHeight() - MaxLadderTopOffset))
	{
		DetachFromLadder(EDetachFromLadderMethod::DLM_ReachingTheTop);
		return;
	}

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
}

void UBaseCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
		case ECustomMovementMode::CMOVE_Mantling:
		{
			PhysMantling(deltaTime, Iterations);
			break;
		}

		case ECustomMovementMode::CMOVE_OnLadder:
		{
			PhysLadder(deltaTime, Iterations);
			break;
		}

		case ECustomMovementMode::CMOVE_OnZipline:
		{
			PhysZipline(deltaTime, Iterations);
			break;
		}

		default:
			break;
	}
	
	Super::PhysCustom(deltaTime, Iterations);
}

void UBaseCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	if (MovementMode == MOVE_Swimming)
	{
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(SwimmingCapsuleRadius, SwimmingCapsuleHalfHeight);
	}
	else if (PreviousMovementMode == MOVE_Swimming)
	{
		const ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);
	}

	if (MovementMode == MOVE_Custom)
	{
		switch (CustomMovementMode)
		{
			case ECustomMovementMode::CMOVE_Mantling:
				{
					GetWorld()->GetTimerManager().SetTimer(MantlingTimer, this, &UBaseCharacterMovementComponent::EndMantle, CurrentMantlingParameters.Duration, false);
					break;
				}

			default:
				break;
		}
	}
}

ABaseCharacter* UBaseCharacterMovementComponent::GetBaseCharacterOwner() const
{
	return StaticCast<ABaseCharacter*>(CharacterOwner);
}
