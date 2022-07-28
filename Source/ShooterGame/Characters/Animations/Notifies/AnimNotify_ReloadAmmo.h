#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ReloadAmmo.generated.h"

UCLASS()
class SHOOTERGAME_API UAnimNotify_ReloadAmmo : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (UIMin=0))
	int32 NumberOfAmmo = 1;
	
};
