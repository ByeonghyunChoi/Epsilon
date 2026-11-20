// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EpsilonCombatLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EPSILON_API UEpsilonCombatLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	static void ApplyDamage(AActor* Instigator, AActor* Target, TSubclassOf<class UGameplayEffect> DamageEffectClass, float DamageAmount);
};
