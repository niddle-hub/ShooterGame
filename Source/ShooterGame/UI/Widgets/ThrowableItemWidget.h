#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ThrowableItemWidget.generated.h"

UCLASS()
class SHOOTERGAME_API UThrowableItemWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item")
	int32 Amount;

private:
	UFUNCTION()
	void UpdateAmount(int32 NewAmount);
	
};
