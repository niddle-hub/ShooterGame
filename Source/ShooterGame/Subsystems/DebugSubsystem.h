#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DebugSubsystem.generated.h"

UCLASS()
class SHOOTERGAME_API UDebugSubsystem final : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	bool IsEnabledCategory(const FName& CategoryName) const;
	
private:
	UFUNCTION(Exec)
	void EnableDebugCategory(const FName& CategoryName, bool NewEnable);

	TMap<FName, bool> Categories;
};
