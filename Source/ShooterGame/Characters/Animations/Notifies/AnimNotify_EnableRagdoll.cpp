#include "AnimNotify_EnableRagdoll.h"
#include "ShooterGame/ShooterGameTypes.h"

void UAnimNotify_EnableRagdoll::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	MeshComp->SetCollisionProfileName(CollisionProfile::Ragdoll);
	MeshComp->SetSimulatePhysics(true);
}
