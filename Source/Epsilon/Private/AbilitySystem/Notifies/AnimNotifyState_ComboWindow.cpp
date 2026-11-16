// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Notifies/AnimNotifyState_ComboWindow.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystem/Abilities/GA_ComboAttack.h"

void UAnimNotifyState_ComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (UGA_ComboAttack* ComboAbility = UGA_ComboAttack::FindActiveInstance(MeshComp->GetOwner()))
	{
		ComboAbility->SetComboWindowOpen(true);
		ComboAbility->TryAdvanceComboIfQueued();
		UE_LOG(LogTemp, Warning, TEXT("WindowOpen"));
	}
}

void UAnimNotifyState_ComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (UGA_ComboAttack* ComboAbility = UGA_ComboAttack::FindActiveInstance(MeshComp->GetOwner()))
	{
		ComboAbility->SetComboWindowOpen(false);
		UE_LOG(LogTemp, Warning, TEXT("WindowClose"));
	}
}
