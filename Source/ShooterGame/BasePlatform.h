#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformInvocator.h"
#include "Components/TimelineComponent.h"
#include "BasePlatform.generated.h"

UENUM()
enum class EPlatformBehavior : uint8 
{
	OnDemand = 0,
	Loop
};

UCLASS()
class SHOOTERGAME_API ABasePlatform : public AActor
{
	GENERATED_BODY()
	
public:
	ABasePlatform();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	void PlatformTimelineUpdate(const float Alpha) const;
	FTimeline PlatformTimeline;

public:

	UFUNCTION(BlueprintCallable)
	void BeginPlatformElevate();

	UFUNCTION(BlueprintCallable)
	void EndPlatformElevate();

	UFUNCTION()
	void OnPlatformInvoked();

protected:

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Platform invocator")
	APlatformInvocator* PlatformInvocator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform settings")
	EPlatformBehavior PlatformBehavior = EPlatformBehavior::OnDemand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform settings")
	UStaticMeshComponent* PlatformMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Platform location")
	FVector StartLocation;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (MakeEditWidget), Category = "Platform location")
	FVector EndLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform data")
	UCurveFloat* TimelineCurve;
};
