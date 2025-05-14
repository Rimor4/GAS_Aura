// Copyright Lucas Wang

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UAttributeMenuController;
class UAttributeSet;
class UAbilitySystemComponent;
struct FWidgetControllerParams;
class UOverlayWidgetController;
class UAuraUserWidget;
/**
 * 
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()
public:
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);
	
	UOverlayWidgetController* GetOrCreateOverlayWidgetController(const FWidgetControllerParams& WCParams);

	UOverlayWidgetController* GetOverlayWidgetController() const { return OverlayWidgetController; }

	UAttributeMenuController* GetOrCreateAttributeMenuController(const FWidgetControllerParams& WCParams);
	
	UAttributeMenuController* GetAttributeMenuController() const { return AttributeMenuController; }

private:
	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UAttributeMenuController> AttributeMenuController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuController> AttributeMenuControllerClass;
};
