// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EpsilonCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/EpsilonAttributeSet.h"

// Sets default values
AEpsilonCharacter::AEpsilonCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UEpsilonAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AEpsilonCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AEpsilonCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		GiveStartupAbilities();
	}
}

void AEpsilonCharacter::GiveStartupAbilities()
{
	if (!AbilitySystemComponent) return;

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


