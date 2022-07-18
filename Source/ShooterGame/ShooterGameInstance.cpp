#include "ShooterGameInstance.h"

bool UShooterGameInstance::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	bool Result = Super::ProcessConsoleExec(Cmd, Ar, Executor);
	if (!Result)
	{
		TArray<UGameInstanceSubsystem*> AllSubsystems = GetSubsystemArray<UGameInstanceSubsystem>();
		for (UGameInstanceSubsystem* Subsystem : AllSubsystems)
		{
			Result |= Subsystem->ProcessConsoleExec(Cmd, Ar, Executor);
		}
	}

	return Result;
}
