#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#if WITH_EDITORONLY_DATA
#include "Components/ArrowComponent.h"
#endif
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameCodeBasePawn.generated.h"

UCLASS()
class SHOOTERGAME_API AGameCodeBasePawn : public APawn
{
	GENERATED_BODY()

public:
	AGameCodeBasePawn();

	UPROPERTY(VisibleAnywhere)
	class UPawnMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionComponent;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Jump();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetInputForward() const { return InputForward; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetInputRight() const { return InputRight; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base pawn")
	float CollisionSphereRadius = 50.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base pawn")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base pawn")
	UCameraComponent* CameraComponent;
	
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base pawn")
	UArrowComponent* ArrowComponent;
#endif

private:
	UFUNCTION()
	void OnBlendComplete();

	UPROPERTY()
	AActor* CurrentViewActor;

	float InputForward = 0.0f;
	float InputRight = 0.0f;
};
