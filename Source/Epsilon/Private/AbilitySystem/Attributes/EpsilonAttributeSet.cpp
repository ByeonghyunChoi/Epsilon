#include "AbilitySystem/Attributes/EpsilonAttributeSet.h"

UEpsilonAttributeSet::UEpsilonAttributeSet()
{
	Health.SetBaseValue(100.f);
	Health.SetCurrentValue(100.f);
	MaxHealth.SetBaseValue(100.f);
	MaxHealth.SetCurrentValue(100.f);
}
