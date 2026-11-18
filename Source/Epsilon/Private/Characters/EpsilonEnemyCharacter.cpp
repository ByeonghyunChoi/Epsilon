// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EpsilonEnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/EpsilonEnemyAttributeSet.h"


AEpsilonEnemyCharacter::AEpsilonEnemyCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UEpsilonEnemyAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AEpsilonEnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void AEpsilonEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		GiveStartupAbilities();
	}
}

void AEpsilonEnemyCharacter::GiveStartupAbilities()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
	{
		if (AbilityClass)
		{
			AbilitySystemComponent->GiveAbility(
				FGameplayAbilitySpec(AbilityClass, 1, 0, this)
			);
		}
	}
}


