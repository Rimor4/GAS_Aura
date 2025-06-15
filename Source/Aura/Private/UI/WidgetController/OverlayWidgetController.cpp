// Copyright Lucas Wang


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraConstants.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "Engine/AssetManager.h"
#include "Game/AuraGameInstanceBase.h"
#include "UI/Widget/AuraUserWidget.h"
#include "Util/AuraUtils.h"


struct FStreamableManager;

void UOverlayWidgetController::InitWidget()
{
	checkf(!OverlayWidgetClass.IsNull(), TEXT("Overlay Widget Class unassigned, please fill out BP_AuraHUD"));

	// todo: 统一管理异步加载和延迟显示
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	TWeakObjectPtr WeakThis(this);
	Streamable.RequestAsyncLoad(OverlayWidgetClass.ToSoftObjectPath(), [WeakThis]()
	{
		if (!WeakThis.IsValid()) return;

		UOverlayWidgetController* This = WeakThis.Get();
		if (UClass* LoadedClass = This->OverlayWidgetClass.Get())
		{
			UWorld* World = This->GetWorld();
			if (!IsValid(World)) return;
			
			This->OverlayWidget = CreateWidget<UAuraUserWidget>(World, LoadedClass);
			if (This->OverlayWidget)
			{
				This->OverlayWidget->SetWidgetController(This);
				This->BroadcastInitialValues();
				
				World->GetTimerManager().SetTimerForNextTick([WeakThis]()
				{
					if (!WeakThis.IsValid()) return;
					
					WeakThis.Get()->OverlayWidget->AddToViewport();
				});
			}
		}
	});
}

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		});

	// 绑定Tag事件，触发时创建MessageWidget
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				// 找到和输入FName相符的Tag
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));

				if (Tag.MatchesTag(MessageTag))
				{
					if (const FUIWidgetRow* Row = FAuraUtils::GetDataTableRow<FUIWidgetRow>(this,
						FName(DataTableName::MessageWidget), Tag.GetTagName()))
					{
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
		});
}
