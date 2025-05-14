// Copyright Lucas Wang


#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AttributeMenuController.h"
#include "UI/WidgetController/OverlayWidgetController.h"

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	OverlayWidgetController = GetOrCreateOverlayWidgetController(WidgetControllerParams);
	OverlayWidgetController->InitWidget();
}

UOverlayWidgetController* AAuraHUD::GetOrCreateOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	return OverlayWidgetController;
}

UAttributeMenuController* AAuraHUD::GetOrCreateAttributeMenuController(const FWidgetControllerParams& WCParams)
{
	if (AttributeMenuController == nullptr)
	{
		AttributeMenuController = NewObject<UAttributeMenuController>(this, AttributeMenuControllerClass);
		AttributeMenuController->SetWidgetControllerParams(WCParams);
		AttributeMenuController->BindCallbacksToDependencies();
	}
	return AttributeMenuController;
}
