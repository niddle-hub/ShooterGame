#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterGame/Characters/BaseCharacter.h"
#include "BaseCharacterController.generated.h"

UCLASS()
class SHOOTERGAME_API ABaseCharacterController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void SetPawn(APawn* InPawn) override;

	bool GetIgnoreCameraPitch() const { return IgnoreCameraPitch; }
	void SetIgnoreCameraPitch(const bool NewIgnore) { IgnoreCameraPitch = NewIgnore; }
	
protected:
	virtual void SetupInputComponent() override;

private:
	TSoftObjectPtr<ABaseCharacter> CachedCharacter;
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	
	void Turn(float Value);
	void LookUp(float Value);
	void TurnAtRate(float Value);
	void LookUpAtRate(float Value);

	void Mantle();
	void Jump();
	void ClimbLadderUp(float Value);
	void InteractWithLadder();
	void Zipline();

	void ChangeCrouchState();
	
	void StartSprint();
	void EndSprint();

	void SwimForward(float Value);
	void SwimRight(float Value);
	void SwimUp(float Value);

	void StartFire();
	void StopFire();

	bool IgnoreCameraPitch = false;
};
