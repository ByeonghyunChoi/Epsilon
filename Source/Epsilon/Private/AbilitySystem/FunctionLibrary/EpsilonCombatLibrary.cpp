// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/FunctionLibrary/EpsilonCombatLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"

void UEpsilonCombatLibrary::ApplyDamage(AActor* Instigator, AActor* Target, TSubclassOf<UGameplayEffect> DamageEffectClass, float DamageAmount)
{
	if (!Target || !DamageEffectClass) return;

	IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(Target);
	if (!TargetASI) return;

	UAbilitySystemComponent* TargetASC = TargetASI->GetAbilitySystemComponent();
	if (!TargetASC) return;

	FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
	ContextHandle.AddInstigator(Instigator, Instigator);

	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffectClass, 1.f, ContextHandle);
	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), -DamageAmount);
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}
