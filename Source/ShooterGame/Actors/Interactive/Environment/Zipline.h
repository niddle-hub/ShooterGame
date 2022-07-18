#pragma once

#include "CoreMinimal.h"
#include "ShooterGame/Actors/Interactive/InteractiveActor.h"
#include "Zipline.generated.h"

class UCapsuleComponent;
UCLASS(Blueprintable)
class SHOOTERGAME_API AZipline : public AInteractiveActor
{
	GENERATED_BODY()

public:
	AZipline();

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	FORCEINLINE float GetCableLenght() const { return CableLenght; }

	FORCEINLINE bool IsOnZipline() const { return OnZipline; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline settings", meta = (UIMin = 0.f, ClampMin = 0.f))
	float ZiplinePillarHeight = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline settings", meta = (UIMin = 0.f, ClampMin = 0.f))
	float ZiplineLenght = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline settings", meta = (UIMin = 0.f, ClampMin = 0.f))
	float CableTopOffset = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline settings")
	float EndPillarOffsetZ = 0.f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Zipline")
	UStaticMeshComponent* StartPillarMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Zipline")
	UStaticMeshComponent* EndPillarMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Zipline")
	UStaticMeshComponent* CableMeshComponent;

	UCapsuleComponent* GetZiplineInteractionVolume() const;

	virtual void OnInteractionVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                             const FHitResult& SweepResult) override;

	virtual void OnInteractionVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:
	void SetPillarMeshScaleZ(UStaticMeshComponent* PillarMeshComponent) const;
	bool OnZipline = false;
	float CableLenght = 0.f;
};
