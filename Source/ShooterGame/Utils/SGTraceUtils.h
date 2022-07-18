#pragma once

namespace SGTraceUtils
{
	bool SweepCapsuleSingleByChannel(
		const UWorld* World,
		FHitResult& OutHit,
		const FVector& Start,
		const FVector& End,
		float CapsuleRadius,
		float CapsuleHalfHeight,
		const FQuat& Rot,
		ECollisionChannel TraceChannel,
		const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam,
		const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam,
		bool DrawDebug = false,
		float DrawDebugTime = -1.f,
		FColor TraceColor = FColor::Black,
		FColor HitColor = FColor::Red);

	bool SweepSphereSingleByChannel(
		const UWorld* World,
		FHitResult& OutHit,
		const FVector& Start,
		const FVector& End,
		float SphereRadius,
		ECollisionChannel TraceChannel,
		const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam,
		const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam,
		bool DrawDebug = false,
		float DrawDebugTime = -1.f,
		FColor TraceColor = FColor::Black,
		FColor HitColor = FColor::Red);

	bool OverlapCapsuleAnyTestByProfile(
		const UWorld* World,
		const FVector& Position,
		float CapsuleRadius,
		float CapsuleHalfHeight,
		FQuat Rotation,
		FName ProfileName,
		const FCollisionQueryParams& QueryParams,
		bool DrawDebug = false,
		float DrawDebugTime = -1.f,
		FColor HitColor = FColor::Red);

	bool OverlapCapsuleBlockingTestByProfile(
		const UWorld* World,
		const FVector& Position,
		float CapsuleRadius,
		float CapsuleHalfHeight,
		FQuat Rotation,
		FName ProfileName,
		const FCollisionQueryParams& QueryParams,
		bool DrawDebug = false,
		float DrawDebugTime = -1.f,
		FColor HitColor = FColor::Red);
}
