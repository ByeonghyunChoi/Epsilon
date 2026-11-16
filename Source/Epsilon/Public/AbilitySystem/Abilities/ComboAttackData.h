#pragma once

#include "CoreMinimal.h"
#include "ComboAttackData.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FComboAttackData
{
	GENERATED_BODY()

	// 섹션 방식(몽타주 하나에 섹션만 나눔)이면 모든 타수가 같은 몽타주를 가리키고 SectionName만 다름
	// 몽타주를 타수별로 분리하는 방식이면 Montage가 각각 다르고 SectionName은 비워둠
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	TObjectPtr<UAnimMontage> Montage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	FName SectionName = NAME_None;
};
