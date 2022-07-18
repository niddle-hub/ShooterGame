#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformInvocator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInvocatorActivated);

UCLASS()
class SHOOTERGAME_API APlatformInvocator : public AActor
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	FOnInvocatorActivated OnInvocatorActivated;

protected:

	UFUNCTION(BlueprintCallable)
	void Invoke();

};
