// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EpsilonTargetingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EPSILON_API UEpsilonTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEpsilonTargetingComponent();

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	AActor* FindAttackTarget() const;

	// 락이 없으면 새로 탐색해서 락을 건다. 콤보/액션 체인의 "첫 타"에서 호출
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	AActor* AcquireNewLock();

	// 지금 락된 타겟을 반환. 없거나 죽었으면 새로 탐색해서 락을 건 뒤 반환
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	AActor* GetOrRefreshLockedTarget();

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void ClearLock();

	// Warp Target을 현재 락된 타겟 기준으로 갱신. 몽타주 재생 직전마다 호출
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	bool UpdateMotionWarpTarget(FName WarpTargetName, float AttackReachDistance, float MaxWarpDistance);

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float SearchRadius = 600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float SearchHalfAngle = 60.f;

protected:
	UPROPERTY()
	TObjectPtr<AActor> LockedTarget = nullptr;
};
