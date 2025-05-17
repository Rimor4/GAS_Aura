// Copyright Lucas Wang

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template <class UserClass, typename PressFuncType, typename ReleaseFuncType, typename HeldFuncType>
	void BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, PressFuncType PressFunc,
	                        ReleaseFuncType ReleaseFunc, HeldFuncType HeldFunc);
};

template <class UserClass, typename PressFuncType, typename ReleaseFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object,
	PressFuncType PressFunc, ReleaseFuncType ReleaseFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);

	for (const auto& [InputTag, Action] : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && InputTag.IsValid())
		{
			if (PressFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressFunc, InputTag);
			}
			
			if (ReleaseFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleaseFunc, InputTag);
			}

			if (HeldFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, InputTag);
			}
		}
	}
}
