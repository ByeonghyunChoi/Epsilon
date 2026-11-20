// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/EpsilonEnemyAttributeSet.h"
#include "GameplayEffectExtension.h"


UEpsilonEnemyAttributeSet::UEpsilonEnemyAttributeSet()
{
	Health.SetBaseValue(100.f);
	Health.SetCurrentValue(100.f);
	MaxHealth.SetBaseValue(100.f);
	MaxHealth.SetCurrentValue(100.f);
}

void UEpsilonEnemyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}

void UEpsilonEnemyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));

		UE_LOG(LogTemp, Warning, TEXT("%s Health: %f / %f"),
			*Data.Target.GetAvatarActor()->GetName(), GetHealth(), GetMaxHealth());
	}
}
