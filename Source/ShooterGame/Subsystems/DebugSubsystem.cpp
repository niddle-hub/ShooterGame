#include "DebugSubsystem.h"

bool UDebugSubsystem::IsEnabledCategory(const FName& CategoryName) const
{
	const bool* IsEnabled = Categories.Find(CategoryName);
	return IsEnabled != nullptr && *IsEnabled;
}

void UDebugSubsystem::EnableDebugCategory(const FName& CategoryName, const bool NewEnable)
{
	Categories.FindOrAdd(CategoryName);
	Categories[CategoryName] = NewEnable;
}