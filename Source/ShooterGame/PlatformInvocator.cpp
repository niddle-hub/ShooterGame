#include "PlatformInvocator.h"

void APlatformInvocator::Invoke()
{
	if (OnInvocatorActivated.IsBound())
	{
		OnInvocatorActivated.Broadcast();
	}
}

