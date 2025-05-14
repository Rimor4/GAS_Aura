// Copyright Lucas Wang


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::GetAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	if (!AbilityInputActions.Contains(InputTag))
	{
		if (bLogNotFound)
		{
			UE_LOG(LogTemp, Error, TEXT("Can't find Info for AttributeTag [%s] on AttributeInfo [%s]"), *InputTag.ToString(), *GetNameSafe(this));
		}
		return nullptr;
	}
	return AbilityInputActions[InputTag].InputAction;
}
