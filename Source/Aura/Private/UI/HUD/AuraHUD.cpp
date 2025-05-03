// Copyright Lucas Wang


#include "UI/HUD/AuraHUD.h"
#include "Engine/AssetManager.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(!OverlayWidgetClass.IsNull(), TEXT("Overlay Widget Class unassigned, please fill out BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass,
	       TEXT("Overlay Widget Controller Class uninitialized, please fill out BP_AuraHUD"));
	
	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

	// TODO: 统一管理异步加载和延迟显示
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(OverlayWidgetClass.ToSoftObjectPath(), [this, WidgetController]()
	{
		if (UClass* LoadedClass = OverlayWidgetClass.Get())
		{
			OverlayWidget = CreateWidget<UAuraUserWidget>(GetWorld(), LoadedClass);
			if (OverlayWidget)
			{
				OverlayWidget->SetWidgetController(WidgetController);
				WidgetController->BroadcastInitialValues();
				
				GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
				{
					OverlayWidget->AddToViewport();
				});
			}
		}
	});
}

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbackToDependencies();
	}
	return OverlayWidgetController;
}
