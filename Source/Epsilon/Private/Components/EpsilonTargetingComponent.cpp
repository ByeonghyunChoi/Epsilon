// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EpsilonTargetingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "MotionWarpingComponent.h"

UEpsilonTargetingComponent::UEpsilonTargetingComponent()
{

}

AActor* UEpsilonTargetingComponent::FindAttackTarget() const
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return nullptr;

	const FVector OwnerLocation = OwnerActor->GetActorLocation();
	const FVector OwnerForward = OwnerActor->GetActorForwardVector();

	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerActor);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	UKismetSystemLibrary::SphereTraceMultiForObjects(
		OwnerActor,
		OwnerLocation,
		OwnerLocation, // 시작/끝 지점이 같으면 그 자리에서 구 형태로 탐색
		SearchRadius,
		ObjectTypes, // 필요한 Object Type 설정
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResults,
		true
	);

	AActor* BestTarget = nullptr;
	float BestDistSq = FLT_MAX;

	for (const FHitResult& Hit : HitResults)
	{
		AActor* Candidate = Hit.GetActor();
		if (!Candidate || Candidate == OwnerActor) continue;

		const FVector ToTarget = Candidate->GetActorLocation() - OwnerLocation;
		const FVector ToTargetDir = ToTarget.GetSafeNormal();

		const float DotProduct = FVector::DotProduct(OwnerForward, ToTargetDir);
		const float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

		if (AngleDeg > SearchHalfAngle) continue; // 콘 범위 밖

		const float DistSq = ToTarget.SizeSquared();
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestTarget = Candidate;
		}
	}

	return BestTarget;
}

AActor* UEpsilonTargetingComponent::AcquireNewLock()
{
	LockedTarget = FindAttackTarget();
	return LockedTarget;
}

AActor* UEpsilonTargetingComponent::GetOrRefreshLockedTarget()
{
	if (!IsValid(LockedTarget))
	{
		AcquireNewLock();
	}
	return LockedTarget;
}

void UEpsilonTargetingComponent::ClearLock()
{
	LockedTarget = nullptr;
}

bool UEpsilonTargetingComponent::UpdateMotionWarpTarget(FName WarpTargetName, float AttackReachDistance, float MaxWarpDistance)
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return false;

	UMotionWarpingComponent* WarpComp = OwnerActor->FindComponentByClass<UMotionWarpingComponent>();
	if (!WarpComp) return false;

	AActor* Target = GetOrRefreshLockedTarget();
	if (!IsValid(Target))
	{
		WarpComp->RemoveWarpTarget(WarpTargetName);
		return false;
	}

	const FVector OwnerLocation = OwnerActor->GetActorLocation();
	const FVector TargetLocation = Target->GetActorLocation();
	const float Distance = FVector::Dist(OwnerLocation, TargetLocation);

	if (Distance > MaxWarpDistance)
	{
		WarpComp->RemoveWarpTarget(WarpTargetName);
		return false;
	}

	const FVector DirectionToTarget = (TargetLocation - OwnerLocation).GetSafeNormal();
	const FVector WarpLocation = TargetLocation - (DirectionToTarget * AttackReachDistance);
	const FRotator WarpRotation = DirectionToTarget.Rotation();

	WarpComp->AddOrUpdateWarpTargetFromLocationAndRotation(WarpTargetName, WarpLocation, WarpRotation);
	return true;
}


