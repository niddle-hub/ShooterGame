#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LedgeDetectorComponent.generated.h"

USTRUCT(BlueprintType)
struct FLedgeDescription
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Location;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPrimitiveComponent* PrimitiveComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRotator Rotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector LedgeNormal;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTERGAME_API ULedgeDetectorComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
    bool DetectLedge (OUT FLedgeDescription& LedgeDescription) const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float MinLedgeHeight = 40.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float MaxLedgeHeight = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float ForwardCheckDistance = 100.f;

private:
	TWeakObjectPtr<ACharacter> CachedCharacterOwner;
	
};
