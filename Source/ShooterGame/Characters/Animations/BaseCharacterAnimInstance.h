#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterGame/ShooterGameTypes.h"
#include "ShooterGame/Characters/BaseCharacter.h"
#include "BaseCharacterAnimInstance.generated.h"

UCLASS()
class SHOOTERGAME_API UBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement", meta = (UIMin = 0.f))
	float Speed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement")
	float WalkSpeed = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement")
	bool IsFalling = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement")
	bool IsCrouching = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement")
	bool IsSprinting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement")
	bool IsOutOfStamina = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement")
	bool IsSwimming = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement")
	bool IsOnLadder = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement")
	float LadderSpeedRatio = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement")
	bool IsStrafing = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement", meta = (UIMin = -180.f, UIMax = 180.f))
	float Direction = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Animation")
	FRotator AimingRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "Character|IK settings")
	FVector RightFootEffectorLocation = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "Character|IK settings")
	FVector LeftFootEffectorLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "Character|IK settings")
	FVector PelvisBoneTransitionOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Weapon")
	EEquipableItemType EquippedItemType = EEquipableItemType::EIT_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Weapon")
	FTransform ForeGripSocketTransform;

private:
	TWeakObjectPtr<ABaseCharacter> CachedCharacter;
};
