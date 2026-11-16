#include "AbilitySystem/Abilities/GA_BasicAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_BasicAttack::UGA_BasicAttack()
{
	// 이 Ability 자신을 식별하는 태그 (입력 쪽에서 TryActivateAbilitiesByTag로 찾는 바로 그 태그)
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Basic")));

	// 활성화되어 있는 동안만 자동으로 붙는 태그
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Attack.Active")));

	// 이 태그가 이미 있으면 재활성화 거부 (공격 중 재입력 방지)
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Attack.Active")));
}

void UGA_BasicAttack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!AttackMontage || !ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* Task =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, FName("PlayAttackMontage"), AttackMontage);

	Task->OnCompleted.AddDynamic(this, &UGA_BasicAttack::K2_EndAbility);
	Task->OnInterrupted.AddDynamic(this, &UGA_BasicAttack::K2_EndAbility);
	Task->OnCancelled.AddDynamic(this, &UGA_BasicAttack::K2_EndAbility);
	Task->ReadyForActivation();
}
