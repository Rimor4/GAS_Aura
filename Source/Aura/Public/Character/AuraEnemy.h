// Copyright Lucas Wang

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

inline constexpr int32 HighlightDepthStencilValue = 250;

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
	
public:
	AAuraEnemy();

#pragma region Enemy Interface
	
	virtual void HighlightActor() override;
	
	virtual void UnHighlightActor() override;
	
#pragma endregion

	
#pragma region Combat Interface
	
	virtual int32 GetPlayerLevel() override;
	
#pragma endregion 
	
protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;

};
