#include "FPPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Controllers/BaseCharacterController.h"
#include "ShooterGame/ShooterGameTypes.h"

AFPPlayerCharacter::AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
	FirstPersonMeshComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonMeshComponent->SetRelativeLocation(FVector(0.f,0.f, -86.f));
	FirstPersonMeshComponent->CastShadow = false;
	FirstPersonMeshComponent->bCastDynamicShadow = false;
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);
	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMeshComponent, SocketFPCamera);
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->bCastHiddenShadow = true;

	CameraComponent->bAutoActivate = false;
	SpringArmComponent->bAutoActivate = false;
	SpringArmComponent->bUsePawnControlRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;
}

void AFPPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (IsFPMontagePlaying() && BaseController.IsValid())
	{
		FRotator TargetControlRotation = BaseController->GetControlRotation();
		TargetControlRotation.Pitch = 0.f;
		constexpr float ControlRotationBlendSpeed = 300.f;
		TargetControlRotation = FMath::RInterpTo(BaseController->GetControlRotation(), TargetControlRotation, DeltaSeconds, ControlRotationBlendSpeed);
		BaseController->SetControlRotation(TargetControlRotation);
	}
}

void AFPPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	const AFPPlayerCharacter* DefaultCharacter = GetDefault<AFPPlayerCharacter>(GetClass());
	FVector& FirstPersonMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable();
	FirstPersonMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z + HalfHeightAdjust;
}

void AFPPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	const AFPPlayerCharacter* DefaultCharacter = GetDefault<AFPPlayerCharacter>(GetClass());
	FVector& FirstPersonMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable();
	FirstPersonMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z;
}

void AFPPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	BaseController = Cast<ABaseCharacterController>(NewController);
}

void AFPPlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	
	if (!BaseController.IsValid())
	{
		return;
	}

	APlayerCameraManager* PlayerCameraManager = BaseController->PlayerCameraManager;
	
	if (GetBaseCharacterMovementComponent()->IsOnLadder())
	{
		BaseController->SetIgnoreCameraPitch(true);
		bUseControllerRotationYaw = false;
		
		 PlayerCameraManager->ViewPitchMin = LadderCameraMinPitch;
		 PlayerCameraManager->ViewPitchMax = LadderCameraMaxPitch;
		 PlayerCameraManager->ViewYawMin = LadderCameraMinYaw;
		 PlayerCameraManager->ViewYawMax = LadderCameraMaxYaw;
	}
	else if (PreviousCustomMode == static_cast<uint8>(ECustomMovementMode::CMOVE_OnLadder))
	{
		BaseController->SetIgnoreCameraPitch(false);
		bUseControllerRotationYaw = true;
		
		 const APlayerCameraManager* DefaultPlayerCameraManager = PlayerCameraManager->GetClass()->GetDefaultObject<APlayerCameraManager>();
		
		 PlayerCameraManager->ViewPitchMin = DefaultPlayerCameraManager->ViewPitchMin;
		 PlayerCameraManager->ViewPitchMax = DefaultPlayerCameraManager->ViewPitchMax;
		 PlayerCameraManager->ViewYawMin = DefaultPlayerCameraManager->ViewYawMin;
		 PlayerCameraManager->ViewYawMax = DefaultPlayerCameraManager->ViewYawMax;
	}
}

bool AFPPlayerCharacter::IsFPMontagePlaying() const
{
	const UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	return IsValid(FPAnimInstance) && FPAnimInstance->IsAnyMontagePlaying();
}

FRotator AFPPlayerCharacter::GetViewRotation() const
{
	FRotator Result = Super::GetViewRotation();
	
	if (IsFPMontagePlaying())
	{
		const FRotator SocketRotation = FirstPersonMeshComponent->GetSocketRotation(SocketFPCamera);
		Result.Pitch += SocketRotation.Pitch;
		Result.Yaw = SocketRotation.Yaw;
		Result.Roll = SocketRotation.Roll;
	}
	
	return Result;
}

void AFPPlayerCharacter::OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime)
{
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	if(IsValid(FPAnimInstance) && MantlingSettings.FPMantlingMontage)
	{
		if (BaseController.IsValid())
		{
			BaseController->SetIgnoreLookInput(true);
			BaseController->SetIgnoreMoveInput(true);
		}
		const float MontageDuration = FPAnimInstance->Montage_Play(MantlingSettings.FPMantlingMontage, 1.f, EMontagePlayReturnType::Duration, MantlingAnimationStartTime);
		GetWorld()->GetTimerManager().SetTimer(FPMontageTimer, this, &AFPPlayerCharacter::OnFPMontageTimerElapsed, MontageDuration, false);
	}
}

void AFPPlayerCharacter::OnFPMontageTimerElapsed() const
{
	if (BaseController.IsValid())
	{
		BaseController->SetIgnoreLookInput(false);
		BaseController->SetIgnoreMoveInput(false);
	}
}
