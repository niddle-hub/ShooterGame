#pragma once

#include "CoreMinimal.h"
#include "ShooterGame/Actors/Interactive/InteractiveActor.h"
#include "Ladder.generated.h"

class UBoxComponent;
UCLASS(Blueprintable)
class SHOOTERGAME_API ALadder : public AInteractiveActor
{
	GENERATED_BODY()

public:
	ALadder();

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	FORCEINLINE float GetLadderHeight() const { return LadderHeight; }

	FORCEINLINE bool IsOnTop() const { return OnTop; }

	FORCEINLINE UAnimMontage* GetAttachFromTopAnimMontage() const { return AttachFromTopAnimMontage; }

	FVector GetAttachFromTopAnimMontageStartLocation() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder settings")
	float LadderHeight = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder settings")
	float LadderWidth = 50.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder settings")
	float StepsInterval = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder settings")
	float BottomStepOffset = 10.f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ladder")
	UStaticMeshComponent* RightRailMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ladder")
	UStaticMeshComponent* LeftRailMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ladder")
	UInstancedStaticMeshComponent* StepMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Ladder")
	UBoxComponent* TopInteractionVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder settings", meta = (DisplayName = "Get Down Animation"))
	UAnimMontage* AttachFromTopAnimMontage;

	//Offset from ladder's top for starting anim montage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder settings")
	FVector AttachFromTopInitialOffset = FVector::ZeroVector;

	UBoxComponent* GetLadderInteractionBox() const;

	virtual void OnInteractionVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnInteractionVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:
	void SetRailMeshScaleZ(UStaticMeshComponent* RailMeshComponent) const;

	bool OnTop = false;
};
