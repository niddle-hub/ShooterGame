#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameCodePawnMovementComponent.generated.h"

UCLASS()
class SHOOTERGAME_API UGameCodePawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void BeginJump();

	FORCEINLINE virtual bool IsFalling() const override { return IsFallingDown; }

protected:
	UPROPERTY(EditAnywhere)
	float MaxSpeed = 1200.f;

	UPROPERTY(EditAnywhere)
	float InitialJumpVelocity = 500.f;

	UPROPERTY(EditAnywhere)
	bool HasEnabledGravity = true;

private:
	FVector VerticalVelocity = FVector::ZeroVector;
	
	bool IsFallingDown = false;
};
