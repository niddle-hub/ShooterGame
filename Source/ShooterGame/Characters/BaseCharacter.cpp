#include "BaseCharacter.h"

#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ShooterGame/Actors/Interactive/Environment/Ladder.h"
#include "ShooterGame/Actors/Interactive/Environment/Zipline.h"
#include "ShooterGame/Components/LedgeDetectorComponent.h"

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(CharacterMovementComponentName))
{
	BaseCharacterMovementComponent = StaticCast<UBaseCharacterMovementComponent*>(GetCharacterMovement());
	LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("Ledge Detector"));
	CurrentStamina = MaxStamina;

	GetMesh()->CastShadow = true;
	GetMesh()->bCastDynamicShadow = true;
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TryChangeSprintState();
	UpdateIKSettings(DeltaSeconds);
	UpdateStamina(DeltaSeconds);
	
	GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Red, FString::Printf(TEXT("Max Speed: %f"), BaseCharacterMovementComponent->GetMaxSpeed()));
	GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Orange, FString::Printf(TEXT("Player Speed: %.2f"), BaseCharacterMovementComponent->Velocity.Size()));
	GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Yellow, FString::Printf(TEXT("Stamina: %.2f"), CurrentStamina));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	OnReachedJumpApex.AddDynamic(this, &ABaseCharacter::OnReachedJumpApexHeight);
	OnHardLandedDelegate.AddDynamic(this, &ABaseCharacter::ABaseCharacter::OnHardLanded);
}

void ABaseCharacter::TryChangeSprintState()
{
	if (IsSprintRequested && !BaseCharacterMovementComponent->IsSprinting() && CanSprint())
	{
		BaseCharacterMovementComponent->StartSprint();
		OnSprintStart();
	}

	if (!IsSprintRequested && BaseCharacterMovementComponent->IsSprinting())
	{
		BaseCharacterMovementComponent->EndSprint();
		OnSprintEnd();
	}
}

void ABaseCharacter::StartSprint()
{
	IsSprintRequested = true;
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

void ABaseCharacter::UpdateStamina(float DeltaTime)
{
	if (BaseCharacterMovementComponent->IsSprinting() && BaseCharacterMovementComponent->Velocity.Size() > BaseCharacterMovementComponent->MaxWalkSpeed)
	{
		CurrentStamina -= StaminaConsumptionVelocity * DeltaTime;
	}
	else
	{
		CurrentStamina += StaminaRestoreVelocity * DeltaTime;
	}

	CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);
	
	if (FMath::IsNearlyZero(CurrentStamina))
	{
		OnStaminaOver();
	}
	else if (CurrentStamina == MaxStamina)
	{
		OnStaminaFullCharged();
	}
}

void ABaseCharacter::EndSprint()
{
	IsSprintRequested = false;
}

void ABaseCharacter::Mantle(const bool Force /*= false*/)
{
	if (!(CanMantle() || Force))
	{
		return;
	}
	
	FLedgeDescription LedgeDescription;
	if(LedgeDetectorComponent->DetectLedge(LedgeDescription))
	{
		if (GetBaseCharacterMovementComponent()->IsCrouching())
		{
			UnCrouch();
		}
		
		const FVector CharacterBottom = GetActorLocation() - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * FVector::UpVector;
		
		FMantlingMovementParameters MantlingParameters;
		MantlingParameters.InitialLocation = GetActorLocation();
		MantlingParameters.InitialRotation = GetActorRotation();
		MantlingParameters.TargetLocation = LedgeDescription.Location;
		MantlingParameters.TargetRotation = LedgeDescription.Rotation;
		MantlingParameters.TargetComponent = LedgeDescription.PrimitiveComponent;

		const ACharacter* DefaultCharacter = GetClass()->GetDefaultObject<ACharacter>();
		const UCapsuleComponent* DefaultCapsule = DefaultCharacter->GetCapsuleComponent();
		
		const FVector MantlingTopEdge = MantlingParameters.GetWorldTargetLocation() - FVector::UpVector * DefaultCapsule->GetScaledCapsuleHalfHeight();
		const float MantlingHeight = FMath::Abs ((CharacterBottom - MantlingTopEdge).Z);
		
		const FMantlingSettings& MantlingSettings = GetMantlingSettings(MantlingHeight);
		
		float MinRange, MaxRange = 0.f;
		MantlingSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);
		MantlingParameters.Duration = MaxRange - MinRange;
		MantlingParameters.MantlingCurve = MantlingSettings.MantlingCurve;
	
		const FVector2D SourceRange(MantlingSettings.MinHeight, MantlingSettings.MaxHeight);
		const FVector2D TargetRange(MantlingSettings.MinHeightStartTime, MantlingSettings.MaxHeightStartTime);
		
		MantlingParameters.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);
		MantlingParameters.InitialAnimationLocation = MantlingParameters.GetWorldTargetLocation() - MantlingSettings.AnimationCorrectionZ * FVector::UpVector + MantlingSettings.AnimationCorrectionXY * LedgeDescription.LedgeNormal;
		
		GetBaseCharacterMovementComponent()->StartMantle(MantlingParameters);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MantlingSettings.MantlingMontage, 1.f, EMontagePlayReturnType::Duration, MantlingParameters.StartTime);
		OnMantle(MantlingSettings, MantlingParameters.StartTime);
	}
}

bool ABaseCharacter::CanMantle() const
{
	return !GetBaseCharacterMovementComponent()->IsMantling() && !bStaminaIsOver && !GetBaseCharacterMovementComponent()->IsFalling() && !GetBaseCharacterMovementComponent()->IsOnLadder();
}

void ABaseCharacter::InteractWithLadder()
{
	if (GetBaseCharacterMovementComponent()->IsOnLadder())
	{
		GetBaseCharacterMovementComponent()->DetachFromLadder(EDetachFromLadderMethod::DLM_JumpOff);
	}
	else
	{
		const ALadder* AvailableLadder = GetAvailableLadder();
		if (IsValid(AvailableLadder))
		{
			if (AvailableLadder->IsOnTop())
			{
				PlayAnimMontage(AvailableLadder->GetAttachFromTopAnimMontage());
			}
			GetBaseCharacterMovementComponent()->AttachToLadder(AvailableLadder);
		}
	}
}

void ABaseCharacter::ClimbLadderUp(float Value)
{
	if (GetBaseCharacterMovementComponent()->IsOnLadder() && !FMath::IsNearlyZero(Value))
	{
		const FVector LadderUpVector = GetBaseCharacterMovementComponent()->GetCurrentLadder()->GetActorUpVector();
		AddMovementInput(LadderUpVector, Value);
	}
}

const ALadder* ABaseCharacter::GetAvailableLadder() const
{
	const ALadder* Result = nullptr;
	for (const AInteractiveActor* Ladder: InteractiveActors)
	{
		if (Ladder->IsA<ALadder>())
		{
			Result = StaticCast<const ALadder*>(Ladder);
		}
	}

	return Result;
}

void ABaseCharacter::Zipline()
{
	if (GetBaseCharacterMovementComponent()->IsOnZipline())
	{
		EndZipline();
	}
	else
	{
		StartZipline();
	}
}

void ABaseCharacter::StartZipline()
{
	if (!CanZipline())
	{
		return;
	}

	const AZipline* AvailableZipline = GetAvailableZipline();
	if (IsValid(AvailableZipline))
	{
		GetBaseCharacterMovementComponent()->AttachToZipline(AvailableZipline);
	}
}

void ABaseCharacter::EndZipline()
{
	GetBaseCharacterMovementComponent()->DetachFromZipline(EDetachFromZiplineMethod::DZM_JumpOff);
}

bool ABaseCharacter::CanZipline()
{
	//TODO: GetAvailableZipline() can zipline ?
	return true;
}

const AZipline* ABaseCharacter::GetAvailableZipline() const
{
	const AZipline* Result = nullptr;
	for (const AInteractiveActor* Zipline: InteractiveActors)
	{
		if (Zipline->IsA<AZipline>())
		{
			Result = StaticCast<const AZipline*>(Zipline);
		}
	}

	return Result;
}

void ABaseCharacter::Falling()
{
	Super::Falling();
	BaseCharacterMovementComponent->bNotifyApex = true;
}

void ABaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	const float CurrentActorLocationZ = GetActorLocation().Z;
	const float JumpLenght = JumpApexHeight - CurrentActorLocationZ;
	
	if (JumpLenght >= HardLandingSettings.HardLandingPlayHeight)
	{
		if(OnHardLandedDelegate.IsBound())
		{
			OnHardLandedDelegate.Broadcast();
		}
	}
}

void ABaseCharacter::PlayHardLandingMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	const float MontageDuration = AnimInstance->Montage_Play(HardLandingSettings.HardLandingMontage);

	FTimerHandle HardLandingTimerHandle;
	GetWorldTimerManager().SetTimer(HardLandingTimerHandle, this, &ABaseCharacter::StopHardLandingMontage,MontageDuration, false);
	
	GetController()->SetIgnoreMoveInput(true);
}

void ABaseCharacter::StopHardLandingMontage() const
{
	GetController()->SetIgnoreMoveInput(false);
}

void ABaseCharacter::OnHardLanded_Implementation()
{
	PlayHardLandingMontage();
}

void ABaseCharacter::RegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	InteractiveActors.AddUnique(InteractiveActor);
}

void ABaseCharacter::UnregisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	InteractiveActors.RemoveSingleSwap(InteractiveActor);
}

bool ABaseCharacter::CanJumpInternal_Implementation() const
{
	return bCanJump && Super::CanJumpInternal_Implementation() &&!GetBaseCharacterMovementComponent()->IsMantling();
}

bool ABaseCharacter::CanSprint()
{
	return bCanSprint && !BaseCharacterMovementComponent->IsFalling();
}

void ABaseCharacter::OnStaminaOver()
{
	bCanJump = false;
	bCanSprint = false;
	bStaminaIsOver = true;
	EndSprint();
	OnSprintEnd();
	BaseCharacterMovementComponent->SetStaminaIsOver(true);
}

void ABaseCharacter::OnStaminaFullCharged()
{
	bCanJump = true;
	bCanSprint = true;
	bStaminaIsOver = false;
	BaseCharacterMovementComponent->SetStaminaIsOver(false);
}

void ABaseCharacter::OnReachedJumpApexHeight()
{
	JumpApexHeight = GetActorLocation().Z;
}

const FMantlingSettings& ABaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	return LedgeHeight > LowMantleMaxHeight ? HighMantlingSettings : LowMantlingSettings;
}

void ABaseCharacter::UpdateIKSettings(float DeltaSeconds)
{
	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, CalculateIKOffsetForSocket(RightFootSocketName), DeltaSeconds, IKInterpSpeed);
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, CalculateIKOffsetForSocket(LeftFootSocketName), DeltaSeconds, IKInterpSpeed);
	IKPelvisOffset = FMath::FInterpTo(IKPelvisOffset, CalculateIKPelvisOffset(), DeltaSeconds, IKInterpSpeed);
}

float ABaseCharacter::CalculateIKPelvisOffset() const
{
	return  IKRightFootOffset > IKLeftFootOffset ? -IKRightFootOffset : -IKLeftFootOffset;
}

float ABaseCharacter::CalculateIKOffsetForSocket(const FName& SocketName) const
{
	float Result = 0.f;

	const float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	
	const FVector SocketLocation = GetMesh()->GetSocketLocation(SocketName);
	const FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);
	const FVector TraceEnd = TraceStart - (CapsuleHalfHeight + IKTraceDistance) * FVector::UpVector;

	FHitResult HitResult;
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	
	const FVector FootSizeBox = FVector(1.f, 15.f, 7.f);
	if (UKismetSystemLibrary::BoxTraceSingle(GetWorld(), TraceStart, TraceEnd, FootSizeBox, GetMesh()->GetSocketRotation(SocketName), TraceType, true, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true))
	{
		const float CharacterBottom = TraceStart.Z - CapsuleHalfHeight;
		Result = CharacterBottom - HitResult.Location.Z;
	}
	
	return Result;
}
