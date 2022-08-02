#include "BaseCharacter.h"

#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveVector.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ShooterGame/ShooterGameTypes.h"
#include "ShooterGame/Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "ShooterGame/Actors/Interactive/Environment/Ladder.h"
#include "ShooterGame/Actors/Interactive/Environment/Zipline.h"
#include "ShooterGame/Components/LedgeDetectorComponent.h"
#include "ShooterGame/Components/CharacterComponents/CharacterAttributesComponent.h"
#include "ShooterGame/Components/CharacterComponents/CharacterEquipmentComponent.h"

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(CharacterMovementComponentName))
{
	BaseCharacterMovementComponent = StaticCast<UBaseCharacterMovementComponent*>(GetCharacterMovement());
	LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("Ledge Detector"));

	CharacterAttributesComponent = CreateDefaultSubobject<UCharacterAttributesComponent>(TEXT("Character Attributes"));
	CharacterEquipmentComponent = CreateDefaultSubobject<UCharacterEquipmentComponent>(TEXT("Character Equipment"));

	GetMesh()->CastShadow = true;
	GetMesh()->bCastDynamicShadow = true;
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TryChangeSprintState();
	UpdateIKSettings(DeltaSeconds);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	OnReachedJumpApex.AddDynamic(this, &ABaseCharacter::OnReachedJumpApexHeight);
	OnHardLandedDelegate.AddDynamic(this, &ABaseCharacter::ABaseCharacter::OnHardLanded);
	CharacterAttributesComponent->OnDeathEvent.AddUObject(this, &ABaseCharacter::OnDeath);
	CharacterAttributesComponent->OnOutOfStaminaEvent.AddUObject(this, &ABaseCharacter::OnStaminaHasChanged);
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

	if (IsValid(FallDamageCurve))
	{
		constexpr float UnitScale = 0.01f;
		const float FallHeight = JumpLenght * UnitScale;
		const float DamageAmount = FallDamageCurve->GetFloatValue(FallHeight);
		TakeDamage(DamageAmount, FDamageEvent(), GetController(), Hit.Actor.Get());
	}
}

void ABaseCharacter::PlayHardLandingMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	const float MontageDuration = AnimInstance->Montage_Play(HardLandingSettings.HardLandingMontage);

	FTimerHandle HardLandingTimerHandle;
	GetWorldTimerManager().SetTimer(HardLandingTimerHandle, this, &ABaseCharacter::StopHardLandingMontage,MontageDuration, false);

	GetController()->SetIgnoreMoveInput(true);
	bCanJump = false;
}

void ABaseCharacter::StopHardLandingMontage()
{
	GetController()->SetIgnoreMoveInput(false);
	bCanJump = true;
}

void ABaseCharacter::EnableRagdoll() const
{
	GetMesh()->SetCollisionProfileName(CollisionProfile::Ragdoll);
	GetMesh()->SetSimulatePhysics(true);
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

bool ABaseCharacter::CanFire() const
{
	const bool IsMantling = GetBaseCharacterMovementComponent()->IsMantling();
	const bool IsFalling = GetBaseCharacterMovementComponent()->IsFalling();
	const bool IsSprinting = GetBaseCharacterMovementComponent()->IsSprinting();
	const bool IsOnLadder = GetBaseCharacterMovementComponent()->IsOnLadder();
	const bool IsSwimming = GetBaseCharacterMovementComponent()->IsSwimming();
	const bool IsEquipping = CharacterEquipmentComponent->IsEquipping();
	
	return !IsEquipping && !IsMantling && !IsFalling && !IsSprinting && !IsOnLadder && !IsSwimming;
}

void ABaseCharacter::StartFire() const
{
	if (!CanFire())
	{
		return;
	}
	ARangeWeaponItem* RangeWeaponItem = CharacterEquipmentComponent->GetEquippedRangeWeapon();
	if (IsValid(RangeWeaponItem))
	{
		RangeWeaponItem->StartFire();
	}
}

void ABaseCharacter::StopFire() const
{
	ARangeWeaponItem* RangeWeaponItem = CharacterEquipmentComponent->GetEquippedRangeWeapon();
	if (IsValid(RangeWeaponItem))
	{
		RangeWeaponItem->StopFire();
	}
}

void ABaseCharacter::StartAiming()
{
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetEquippedRangeWeapon();
	if (!IsValid(CurrentRangeWeapon))
	{
		return;
	}
	bIsAiming = true;
	AimingMovementSpeed = CurrentRangeWeapon->GetAimingMaxMovementSpeed();
	CurrentRangeWeapon->StartAiming();
	OnStartAiming();
}

void ABaseCharacter::StopAiming()
{
	if (!bIsAiming)
	{
		return;
	}
	
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetEquippedRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopAiming();
	}
	
	bIsAiming = false;
	AimingMovementSpeed = 0.0f;
	OnStopAiming();
}

void ABaseCharacter::NextItem() const
{
	CharacterEquipmentComponent->EquipNext();
}

void ABaseCharacter::PreviousItem() const
{
	CharacterEquipmentComponent->EquipPrevious();
}

void ABaseCharacter::ReloadEquippedWeapon() const
{
	if(IsValid(GetCharacterEquipmentComponent()->GetEquippedRangeWeapon()))
	{
		CharacterEquipmentComponent->ReloadEquippedWeapon();
	}
}

void ABaseCharacter::EquipPrimaryItem() const
{
	CharacterEquipmentComponent->EquipItemInSlot(EEquipmentSlot::ES_PrimaryItem);
}

void ABaseCharacter::OnStartAiming_Implementation()
{
	OnStartAimingInternal();
	EndSprint();
}

void ABaseCharacter::OnStopAiming_Implementation()
{
	OnStopAimingInternal();
}

float ABaseCharacter::GetAimingMovementSpeed() const
{
	return AimingMovementSpeed;
}

bool ABaseCharacter::CanJumpInternal_Implementation() const
{
	return bCanJump && Super::CanJumpInternal_Implementation() &&!GetBaseCharacterMovementComponent()->IsMantling();
}

void ABaseCharacter::OnDeath()
{
	if (!bIsDead)
	{
		bIsDead = true;
		GetBaseCharacterMovementComponent()->DisableMovement();
		bUseControllerRotationYaw = false;
	
		const float DeathDuration = PlayAnimMontage(OnDeathAnimMontage);
	
		if (DeathDuration == 0.f)
		{
			EnableRagdoll();
		}
	}
}

void ABaseCharacter::OnStartAimingInternal()
{
	if(OnAimingStateChanged.IsBound())
	{
		OnAimingStateChanged.Broadcast(true);
	}
}

void ABaseCharacter::OnStopAimingInternal()
{
	if(OnAimingStateChanged.IsBound())
	{
		OnAimingStateChanged.Broadcast(false);
	}
}

bool ABaseCharacter::CanSprint()
{
	return bCanSprint && !BaseCharacterMovementComponent->IsFalling();
}

void ABaseCharacter::OnStaminaHasChanged(const bool InState)
{
	bCanJump = !InState;
	bCanSprint = !InState;
	bStaminaIsOver = InState;
	BaseCharacterMovementComponent->SetStaminaIsOver(InState);

	if (InState)
	{
		EndSprint();
	}
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
