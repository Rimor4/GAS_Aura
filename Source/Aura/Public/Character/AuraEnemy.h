// Copyright Lucas Wang

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AbilitySystemInterface.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AAuraEnemy();
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
};
