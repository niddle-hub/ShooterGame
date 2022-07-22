#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerCharacter.generated.h"

UCLASS(Blueprintable)
class SHOOTERGAME_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	
	virtual void Turn(float Value) override;
	virtual void LookUp(float Value) override;
	virtual void TurnAtRate(float Value) override;
	virtual void LookUpAtRate(float Value) override;

	virtual void ChangeCrouchState() override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	
	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override;

	virtual bool CanSprint() override;
	virtual void OnSprintStart_Implementation() override;
	virtual void OnSprintEnd_Implementation() override;

	virtual void SwimForward(float Value) override;
	virtual void SwimRight(float Value) override;
	virtual void SwimUp(float Value) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	UCameraComponent* CameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement|Sprint")
	UCurveFloat* SprintTimelineCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Aiming")
	UCurveFloat* AimingTimelineCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement|Sprint", meta = (ClmapMin = 0.f, UIMin = 0.f))
	float DefaultSpringArmLength = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Movement|Sprint", meta = (ClmapMin = 0.f, UIMin = 0.f))
	float TargetSpringArmLength = 500.f;

	virtual void OnStartAimingInternal() override;

	virtual void OnStopAimingInternal() override;

private:
	FTimeline SpringArmSprintTimeline;
	void SpringArmSprintTimelineUpdate(const float Alpha) const;

	FTimeline AimingFOVTimeline;
	void AimingFOVTimelineUpdate(const float Alpha) const;

	float GetAimTurnModifier() const;
	float GetAimLookUpModifier() const;

	float ForwardAxis = 0.f;
	float RightAxis = 0.f;
	bool AreRequiredKeysDown() const;
};