// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Notifies/AnimNotifyState_WeaponTrace.h"
#include "Components/SkeletalMeshComponent.h"
#include "Characters/EpsilonCharacter.h"
#include "Components/EpsilonWeaponComponent.h"

void UAnimNotifyState_WeaponTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	if (AEpsilonCharacter* OwnerChar = Cast<AEpsilonCharacter>(MeshComp->GetOwner()))
	{
		if (UEpsilonWeaponComponent* Weapon = OwnerChar->GetWeaponComponent())
		{
			Weapon->StartHitDetection();
		}
	}
}

void UAnimNotifyState_WeaponTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	if (AEpsilonCharacter* OwnerChar = Cast<AEpsilonCharacter>(MeshComp->GetOwner()))
	{
		if (UEpsilonWeaponComponent* Weapon = OwnerChar->GetWeaponComponent())
		{
			Weapon->StopHitDetection();
		}
	}
}
