#include "SGTraceUtils.h"
#include "DrawDebugHelpers.h"

bool SGTraceUtils::SweepCapsuleSingleByChannel(const UWorld* World, FHitResult& OutHit, const FVector& Start, const FVector& End, const float CapsuleRadius, const float CapsuleHalfHeight, const FQuat& Rot, const ECollisionChannel TraceChannel, const FCollisionQueryParams& Params, const FCollisionResponseParams& ResponseParam, const bool DrawDebug, const float DrawDebugTime, const FColor TraceColor, const FColor HitColor)
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	const bool Result = World->SweepSingleByChannel(OutHit, Start, End, Rot, TraceChannel, CollisionShape, Params, ResponseParam);

#if ENABLE_DRAW_DEBUG
	if (DrawDebug)
	{
		DrawDebugCapsule(World, Start, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity, TraceColor, false, DrawDebugTime);
		DrawDebugCapsule(World, End, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity, TraceColor, false, DrawDebugTime);
		DrawDebugLine(World, Start, End, TraceColor, false, DrawDebugTime);
		
		if (Result)
		{
			DrawDebugCapsule(World, OutHit.Location, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity, HitColor, false, DrawDebugTime);
			DrawDebugPoint(World, OutHit.ImpactPoint, 10.f, HitColor, false, DrawDebugTime);
		}
	}
#endif
	
	return Result;
}

bool SGTraceUtils::SweepSphereSingleByChannel(const UWorld* World, FHitResult& OutHit, const FVector& Start, const FVector& End, const float SphereRadius, const ECollisionChannel TraceChannel, const FCollisionQueryParams& Params, const FCollisionResponseParams& ResponseParam, const bool DrawDebug, const float DrawDebugTime, const FColor TraceColor, const FColor HitColor)
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(SphereRadius);
	const bool Result = World->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, TraceChannel, CollisionShape, Params, ResponseParam);

#if ENABLE_DRAW_DEBUG
	if (DrawDebug)
	{
		const FVector SphereCenter = (Start + End) * 0.5f;
		const FVector TraceVector = End - Start;
		const FQuat SphereRotation = FRotationMatrix::MakeFromZ(TraceVector).ToQuat();
		const float SphereHalfHeight = TraceVector.Size() * 0.5f;
		
		DrawDebugCapsule(World, SphereCenter, SphereHalfHeight, SphereRadius, SphereRotation, TraceColor, false, DrawDebugTime);

		if (Result)
		{
			DrawDebugSphere(World, OutHit.Location, SphereRadius, 32, HitColor, false, DrawDebugTime);
			DrawDebugPoint(World, OutHit.ImpactPoint, 10.f, HitColor, false, DrawDebugTime);
		}
	}
#endif
	
	return Result;
}

bool SGTraceUtils::OverlapCapsuleAnyTestByProfile(const UWorld* World, const FVector& Position, const float CapsuleRadius, const float CapsuleHalfHeight, const FQuat Rotation, const FName ProfileName, const FCollisionQueryParams& QueryParams, const bool DrawDebug, const float DrawDebugTime, const FColor HitColor)
{
	const FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	const bool Result = World->OverlapAnyTestByProfile(Position, Rotation, ProfileName, CapsuleShape, QueryParams);
	
#if ENABLE_DRAW_DEBUG
	if (DrawDebug && Result)
	{
		DrawDebugCapsule(World, Position, CapsuleHalfHeight, CapsuleRadius, Rotation, HitColor, false, DrawDebugTime);
	}
#endif
	
	return Result;
}

bool SGTraceUtils::OverlapCapsuleBlockingTestByProfile(const UWorld* World, const FVector& Position, float CapsuleRadius, float CapsuleHalfHeight, FQuat Rotation, FName ProfileName, const FCollisionQueryParams& QueryParams, bool DrawDebug, float DrawDebugTime, FColor HitColor)
{
	const FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	const bool Result = World->OverlapBlockingTestByProfile(Position, Rotation, ProfileName, CapsuleShape, QueryParams);
	
#if ENABLE_DRAW_DEBUG
	if (DrawDebug && Result)
	{
		DrawDebugCapsule(World, Position, CapsuleHalfHeight, CapsuleRadius, Rotation, HitColor, false, DrawDebugTime);
	}
#endif
	
	return Result;
}
