#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "FPPlayerCharacter.generated.h"

UCLASS()
class SHOOTERGAME_API AFPPlayerCharacter : public APlayerCharacter
{
	GENERATED_BODY()

public:
	AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaSeconds) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	virtual FRotator GetViewRotation() const override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|FirstPerson")
	USkeletalMeshComponent* FirstPersonMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|FirstPerson")
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|FirstPerson|Ladder|Camera|Pitch", meta = (UIMin = -89.f, UIMax = 89.f), DisplayName = "Min view rotation")
	float LadderCameraMinPitch = -60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|FirstPerson|Ladder|Camera|Pitch", meta = (UIMin = -89.f, UIMax = 89.f), DisplayName = "Max view rotation")
	float LadderCameraMaxPitch = 80.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|FirstPerson|Ladder|Camera|Yaw", meta = (UIMin = 0.f, UIMax = 359.f), DisplayName = "Min view rotation")
	float LadderCameraMinYaw = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|FirstPerson|Ladder|Camera|Yaw", meta = (UIMin = 0.f, UIMax = 359.f), DisplayName = "Max view rotation")
	float LadderCameraMaxYaw = 175.f;
	
private:
	FTimerHandle FPMontageTimer;
	TWeakObjectPtr<class ABaseCharacterController> BaseController;
	
	void OnFPMontageTimerElapsed() const;
	bool IsFPMontagePlaying() const;

protected:
	virtual void OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime) override;
};
