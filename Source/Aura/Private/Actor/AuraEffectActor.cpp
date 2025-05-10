// Copyright Lucas Wang


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#pragma region Life Cycle

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

 AAuraEffectActor::~AAuraEffectActor()
{
	for (const auto& Entry : ActiveEffectHandles)
	{
		if (UAbilitySystemComponent* ASC = Entry.Key.Get())
		{
			for (const auto& Handle : Entry.Value)
			{
				ASC->RemoveActiveGameplayEffect(Handle, 1);
			}
		}
	}
	ActiveEffectHandles.Empty();
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

#pragma endregion


void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;
	check(GameplayEffectClass);

	// 初始化 GE 上下文
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);

	// 生成 GE Spec（GE的参数总和）
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(
		GameplayEffectClass, ActorLevel, EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(
		*EffectSpecHandle.Data.Get());

	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy ==
		EGameplayEffectDurationType::Infinite;

	// 如果是永久性且需要在结束重叠时移除的GE，添加到Map中便于后续移除
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		auto & Handles = ActiveEffectHandles.FindOrAdd(TargetASC);
		Handles.Add(ActiveEffectHandle);
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		for (const auto& InstantGameplayEffectClass : InstantGameplayEffectClasses)
		{
			ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
		}
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		for (const auto& DurationGameplayEffectClass : DurationGameplayEffectClasses)
		{
			ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
		}
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		for (const auto& InfiniteGameplayEffectClass : InfiniteGameplayEffectClasses)
		{
			ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
		}
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		for (const auto& InstantGameplayEffectClass : InstantGameplayEffectClasses)
		{
			ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
		}
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		for (const auto& DurationGameplayEffectClass : DurationGameplayEffectClasses)
		{
			ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
		}
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		for (const auto& InfiniteGameplayEffectClass : InfiniteGameplayEffectClasses)
		{
			ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
		}
	}
	// 移除永久性的 GE
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;

		if (ActiveEffectHandles.Contains(TargetASC))
		{
			for (const auto& ActiveEffectHandle : ActiveEffectHandles[TargetASC])
			{
				TargetASC->RemoveActiveGameplayEffect(ActiveEffectHandle, 1);
			}
			ActiveEffectHandles.Remove(TargetASC);
		}
	}
}
