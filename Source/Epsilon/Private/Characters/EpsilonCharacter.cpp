// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EpsilonCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/EpsilonAttributeSet.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"

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

void AEpsilonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (BasicAttackInputAction)
		{
			EIC->BindAction(BasicAttackInputAction, ETriggerEvent::Started, this, &AEpsilonCharacter::OnBasicAttackInput);
		}
	}
}

void AEpsilonCharacter::OnBasicAttackInput(const FInputActionValue& Value)
{
	if (!AbilitySystemComponent) return;

	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Basic")));
	AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
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


