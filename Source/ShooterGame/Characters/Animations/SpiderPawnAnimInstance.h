#pragma once

#include "CoreMinimal.h"
#include "BasePawnAnimInstance.h"
#include "ShooterGame/Characters/SpiderPawn.h"
#include "SpiderPawnAnimInstance.generated.h"

UCLASS()
class SHOOTERGAME_API USpiderPawnAnimInstance : public UBasePawnAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "Spider bot|IK Settings")
	FVector RightFrontFootEffectorLocation = FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "Spider bot|IK Settings")
	FVector RightRearFootEffectorLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "Spider bot|IK Settings")
	FVector LeftFrontFootEffectorLocation = FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "Spider bot|IK Settings")
	FVector LeftRearFootEffectorLocation = FVector::ZeroVector;

private:
	TWeakObjectPtr<class ASpiderPawn> CachedSpiderPawnOwner;
	
};
