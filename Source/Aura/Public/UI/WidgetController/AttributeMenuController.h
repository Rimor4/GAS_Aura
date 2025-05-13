// Copyright Lucas Wang

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuController.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAttributeMenuController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;

	virtual void BindCallbacksToDependencies() override;
};
