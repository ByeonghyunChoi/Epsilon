// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ComboAttackData.h"
#include "GA_ComboAttack.generated.h"

class UAbilityTask_PlayMontageAndWait;

/**
 * 
 */
UCLASS()
class EPSILON_API UGA_ComboAttack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_ComboAttack();

	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	TArray<FComboAttackData> ComboData;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	void SetComboWindowOpen(bool bOpen);
	void RequestNextCombo();
	void TryAdvanceComboIfQueued();

	static UGA_ComboAttack* FindActiveInstance(AActor* OwnerActor);

protected:
	int32 CurrentComboIndex = 0;
	bool bNextComboQueued = false;
	bool bComboWindowOpen = false;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> CurrentMontageTask;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float AttackReachDistance = 150.f; // 적 앞 이 거리만큼 떨어진 지점에서 타격

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float MaxWarpDistance = 600.f; // 이 거리를 넘으면 제자리 공격으로 폴백

	const FGameplayAbilityActorInfo* CachedActorInfo = nullptr;

	UFUNCTION()
	void OnMontageCompleted();

	void PlayComboMontage(int32 ComboIndex);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float DamageAmount = 10.f;

	FDelegateHandle WeaponHitDelegateHandle;

	UFUNCTION()
	void HandleWeaponHit(AActor* HitActor);

};
