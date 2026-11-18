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

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float SearchRadius = 600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float SearchHalfAngle = 60.f;		
};
