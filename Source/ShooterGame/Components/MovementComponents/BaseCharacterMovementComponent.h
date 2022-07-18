// ReSharper disable IdentifierTypo
// ReSharper disable CppUE4CodingStandardNamingViolationWarning
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseCharacterMovementComponent.generated.h"

struct FMantlingMovementParameters
{
	FVector InitialLocation = FVector::ZeroVector;
	FRotator InitialRotation = FRotator::ZeroRotator;

	FVector TargetLocation = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;
	
	TWeakObjectPtr<UPrimitiveComponent> TargetComponent;

	FVector InitialAnimationLocation = FVector::ZeroVector;

	float Duration = 1.f;
	float StartTime = 0.f;

	TWeakObjectPtr<UCurveVector> MantlingCurve;

	FVector GetWorldTargetLocation() const
	{
		return TargetComponent->GetComponentTransform().TransformPosition(TargetLocation);
	}
};

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	CMOVE_None = 0 UMETA(DisplayName = "None"),
	CMOVE_Mantling UMETA(DisplayName = "Mantling"),
	CMOVE_OnLadder UMETA(DisplayName = "On Ladder"),
	CMOVE_OnZipline UMETA(DisplayName = "On Zipline"),
	CMOVE_Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EDetachFromLadderMethod : uint8
{
	DLM_Fall = 0,
	DLM_ReachingTheTop,
	DLM_ReachingTheButtom,
	DLM_JumpOff
};

UENUM(BlueprintType)
enum class EDetachFromZiplineMethod : uint8
{
	DZM_Fall = 0,
	DZM_ReachingTheEnd,
	DZM_JumpOff
};

UCLASS()
class SHOOTERGAME_API UBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	virtual void PhysicsRotation(float DeltaTime) override;
	
	FORCEINLINE bool IsSprinting() const { return bIsSprinting; }
	virtual float GetMaxSpeed() const override;

	void StartSprint();
	void EndSprint();

	void StartMantle(const FMantlingMovementParameters& MantlingMovementParameters);
	void EndMantle();
	bool IsMantling() const;

	void AttachToLadder(const class ALadder* Ladder);
	void DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod = EDetachFromLadderMethod::DLM_Fall);
	bool IsOnLadder() const;

	float GetActorToLadderProjection(const FVector& ActorLocation) const;
	float GetLadderSpeedRatio() const;

	void AttachToZipline(const class AZipline* Zipline);
	void DetachFromZipline(EDetachFromZiplineMethod DetachFromZiplineMethod = EDetachFromZiplineMethod::DZM_Fall);
	bool IsOnZipline() const;

	float GetActorToZiplineProjection(const FVector& ActorLocation) const;

	FORCEINLINE const ALadder* GetCurrentLadder() const { return CurrentLadder; }

	FORCEINLINE bool GetStaminaIsOver() const { return bStaminaIsOver; };
	void SetStaminaIsOver(const bool StaminaIsOver_In) { bStaminaIsOver = StaminaIsOver_In; };

	FORCEINLINE float GetSprintSpeed() const { return SprintSpeed; }

protected:
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	void PhysMantling(float DeltaTime, int32 Iterations);
	void PhysLadder(float DeltaTime, int32 Iterations);
	void PhysZipline(float DeltaTime, int32 Iterations) {};
	
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	class ABaseCharacter* GetBaseCharacterOwner() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement|Sprint", meta = (ClampMin = 0.f, UIMin = 0.f))
	float SprintSpeed = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement|Sprint", meta = (ClampMin = 0.f, UIMin = 0.f))
	float RestWalkSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement|Swimming", meta = (ClampMin = 0.f, UIMin = 0.f))
	float SwimmingCapsuleRadius = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement|Swimming", meta = (ClampMin = 0.f, UIMin = 0.f))
	float SwimmingCapsuleHalfHeight = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement|Ladder", meta = (ClampMin = 0.f, UIMin = 0.f, DisplayName = "Climbing Speed"))
	float ClimbingOnLadderSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement|Ladder", meta = (ClampMin = 0.f, UIMin = 0.f, DisplayName = "Breaking Decelartion"))
	float ClimbingOnLadderBrakingDeceleration = 2048.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement|Ladder", meta = (ClampMin = 0.f, UIMin = 0.f, DisplayName = "Character Offset"))
	float CharacterToLadderOffset = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement|Ladder", meta = (ClampMin = 0.f, UIMin = 0.f))
	float MaxLadderTopOffset = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement|Ladder", meta = (ClampMin = 0.f, UIMin = 0.f))
	float MinLadderBottomOffset = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement|Ladder", meta = (ClampMin = 0.f, UIMin = 0.f))
	float JumpOffLadderSpeed = 500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement|Zipline", meta = (ClampMin = 0.f, UIMin = 0.f, DisplayName = "Zipline Movement Speed"))
	float ZiplineMovementSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement|Zipline", meta = (ClampMin = 0.f, UIMin = 0.f, DisplayName = "Breaking Decelartion"))
	float RidingOnZiplineBrakingDeceleration = 2048.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement|Zippline", meta = (ClampMin = 0.f, UIMin = 0.f))
	float JumpOffZiplineSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement|Zipline", meta = (ClampMin = 0.f, UIMin = 0.f))
	float MinZiplineStartOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement|Zipline", meta = (ClampMin = 0.f, UIMin = 0.f))
	float MaxZiplineEndOffset = 90.f;

private:
	bool bIsSprinting = false;
	bool bStaminaIsOver = false;

	FMantlingMovementParameters CurrentMantlingParameters;
	
	FTimerHandle MantlingTimer;

	// ReSharper disable once CppUE4ProbableMemoryIssuesWithUObject
	const ALadder* CurrentLadder = nullptr;
	// ReSharper disable once CppUE4ProbableMemoryIssuesWithUObject
	const AZipline* CurrentZipline = nullptr;

	FRotator ForceTargetRotation = FRotator::ZeroRotator;
	bool ForceRotation = false;
	
};
