#include "AbilitySystem/Abilities/GA_ComboAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Characters/EpsilonCharacter.h"
#include "Components/EpsilonTargetingComponent.h"
#include "Components/EpsilonWeaponComponent.h"

UGA_ComboAttack::UGA_ComboAttack()
{
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Combo")));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Attack.Active")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Attack.Active")));
}

void UGA_ComboAttack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	CurrentComboIndex = 0;
	bNextComboQueued = false;
	CachedActorInfo = ActorInfo;

	if (!ComboData.IsValidIndex(CurrentComboIndex) || !ComboData[CurrentComboIndex].Montage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (AEpsilonCharacter* OwnerChar = Cast<AEpsilonCharacter>(ActorInfo->AvatarActor.Get()))
	{
		OwnerChar->GetEpsilonTargetingComponent()->AcquireNewLock(); // 콤보 시작 시 1회
	}

	if (AEpsilonCharacter* OwnerChar = Cast<AEpsilonCharacter>(ActorInfo->AvatarActor.Get()))
	{
		if (UEpsilonWeaponComponent* Weapon = OwnerChar->GetWeaponComponent())
		{
			Weapon->OnWeaponHit.AddLambda([](AActor* HitActor)
				{
					UE_LOG(LogTemp, Warning, TEXT("Weapon Hit: %s"), *HitActor->GetName());
				});
		}
	}

	PlayComboMontage(CurrentComboIndex);
}

void UGA_ComboAttack::SetComboWindowOpen(bool bOpen)
{
	bComboWindowOpen = bOpen;

	UE_LOG(LogTemp, Warning, TEXT("ComboWindowOpen changed to: %s"), bOpen ? TEXT("true") : TEXT("false"));
}

void UGA_ComboAttack::RequestNextCombo()
{
	if (bComboWindowOpen)
	{
		bNextComboQueued = true;
		TryAdvanceComboIfQueued(); // 윈도우가 이미 열려있는 중에 입력이 들어온 경우, 바로 전환 시도
	}
	// 윈도우가 닫혀있으면 입력은 그냥 버려짐 (아무것도 안 함)
}

void UGA_ComboAttack::TryAdvanceComboIfQueued()
{
	if (bComboWindowOpen && bNextComboQueued && ComboData.IsValidIndex(CurrentComboIndex + 1))
	{
		CurrentComboIndex++;
		bNextComboQueued = false;
		bComboWindowOpen = false;
		PlayComboMontage(CurrentComboIndex);
	}
}

UGA_ComboAttack* UGA_ComboAttack::FindActiveInstance(AActor* OwnerActor)
{
	if (!OwnerActor) return nullptr;

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwnerActor);
	if (!ASI) return nullptr;

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
	if (!ASC) return nullptr;

	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.IsActive())
		{
			for (UGameplayAbility* Instance : Spec.GetAbilityInstances())
			{
				if (UGA_ComboAttack* ComboInstance = Cast<UGA_ComboAttack>(Instance))
				{
					return ComboInstance;
				}
			}
		}
	}

	return nullptr;
}

void UGA_ComboAttack::PlayComboMontage(int32 ComboIndex)
{
	if (CurrentMontageTask)
	{
		CurrentMontageTask->OnCompleted.RemoveAll(this);
		CurrentMontageTask->OnInterrupted.RemoveAll(this);
		CurrentMontageTask->OnCancelled.RemoveAll(this);
	}

	if (CachedActorInfo)
	{
		if (AEpsilonCharacter* OwnerChar = Cast<AEpsilonCharacter>(CachedActorInfo->AvatarActor.Get()))
		{
			OwnerChar->GetEpsilonTargetingComponent()->UpdateMotionWarpTarget(
				FName("AttackTarget"), AttackReachDistance, MaxWarpDistance);
		}
	}

	CurrentMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, FName("PlayComboMontage"),
		ComboData[ComboIndex].Montage,
		1.0f,
		ComboData[ComboIndex].SectionName
	);

	CurrentMontageTask->OnCompleted.AddDynamic(this, &UGA_ComboAttack::OnMontageCompleted);
	CurrentMontageTask->OnInterrupted.AddDynamic(this, &UGA_ComboAttack::OnMontageCompleted);
	CurrentMontageTask->OnCancelled.AddDynamic(this, &UGA_ComboAttack::OnMontageCompleted);
	CurrentMontageTask->ReadyForActivation();

}

void UGA_ComboAttack::OnMontageCompleted()
{
	CurrentComboIndex = 0;
	bNextComboQueued = false;
	bComboWindowOpen = false;

	if (CachedActorInfo)
	{
		if (AEpsilonCharacter* OwnerChar = Cast<AEpsilonCharacter>(CachedActorInfo->AvatarActor.Get()))
		{
			OwnerChar->GetEpsilonTargetingComponent()->ClearLock();
		}
	}

	K2_EndAbility();
}