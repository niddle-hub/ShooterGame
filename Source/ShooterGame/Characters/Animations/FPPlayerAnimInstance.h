#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterAnimInstance.h"
#include "FPPlayerAnimInstance.generated.h"

UCLASS()
class SHOOTERGAME_API UFPPlayerAnimInstance : public UBaseCharacterAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Animations|FirstPeson")
	float PlayerCameraPitchAngle = 0.f;

	TWeakObjectPtr<class AFPPlayerCharacter> CachedCharacterOwner;

private:
	float CalculateCameraPitchAngle() const;
};
