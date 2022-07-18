#pragma once

#include "CoreMinimal.h"
#include "GameCodeBasePawn.h"
#include "SpiderPawn.generated.h"

UCLASS()
class SHOOTERGAME_API ASpiderPawn : public AGameCodeBasePawn
{
	GENERATED_BODY()
	
public:
	ASpiderPawn();
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetIKRightFrontFootOffset() const { return IKRightFrontFootOffset; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetIKRightRearFootOffset() const{ return IKRightRearFootOffset; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetIKLeftFrontFootOffset() const{ return IKLeftFrontFootOffset; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetIKLeftRearFootOffset() const{ return IKLeftRearFootOffset; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spider bot")
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spider bot|IK settings")
	FName RightFrontFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spider bot|IK settings")
	FName RightRearFootSocketName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spider bot|IK settings")
	FName LeftFrontFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spider bot|IK settings")
	FName LeftRearFootSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spider bot|IK settings", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float IKTraceExtendDistance = 30.0f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Spider bot|IK settings", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float IKInterpSpeed = 20.0f;

private:
	float GetIKOffsetForSocket(const FName& SocketName) const;
	
	float IKRightFrontFootOffset = 0.f;
	float IKRightRearFootOffset = 0.f;
	float IKLeftFrontFootOffset = 0.f;
	float IKLeftRearFootOffset = 0.f;

	float IKTraceDistance = 0.f;
	float IKScale = 0.f;
};
