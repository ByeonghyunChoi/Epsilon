// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/EpsilonEnemyAttributeSet.h"

UEpsilonEnemyAttributeSet::UEpsilonEnemyAttributeSet()
{
	Health.SetBaseValue(100.f);
	Health.SetCurrentValue(100.f);
	MaxHealth.SetBaseValue(100.f);
	MaxHealth.SetCurrentValue(100.f);
}
