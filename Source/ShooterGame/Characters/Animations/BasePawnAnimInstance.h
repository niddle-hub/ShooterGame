#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BasePawnAnimInstance.generated.h"

UCLASS()
class SHOOTERGAME_API UBasePawnAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly, Transient, Category="Pawn animation input alpha")
	float InputForward;
	
	UPROPERTY(BlueprintReadOnly, Transient, Category="Pawn animation input alpha")
	float InputRight;
	
	UPROPERTY(BlueprintReadOnly, Transient, Category="Pawn animation flags")
	bool IsInAir;

private:
	TWeakObjectPtr<class AGameCodeBasePawn> CachedPawn;
};
