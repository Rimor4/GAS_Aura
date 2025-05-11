// Copyright Lucas Wang


#include "AbilitySystem/ModMagCalc/MMC_MaxMana.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxMana::UMMC_MaxMana()
{
	IntelliDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	IntelliDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IntelliDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(IntelliDef);
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParams;
	EvaluationParams.SourceTags = SourceTags;
	EvaluationParams.TargetTags = TargetTags;
	
	float IntelliMagnitude = 0.f;
	GetCapturedAttributeMagnitude(IntelliDef, Spec, EvaluationParams, IntelliMagnitude);
	IntelliMagnitude = FMath::Max<float>(IntelliMagnitude, 0.f);

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

	return 50.f + 2.0f * IntelliMagnitude + 8.f * PlayerLevel;
}
