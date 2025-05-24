// Copyright Lucas Wang

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AuraGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIsTopDownChanged, bool, bIsTopDown);

/**
 * 
 */
UCLASS()
class AURA_API AAuraGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IsTopDown, Category=Game)
	bool bIsTopDown;

	UPROPERTY(BlueprintAssignable, Category=Game)
	FOnIsTopDownChanged OnIsTopDownChanged;

	UFUNCTION()
	void OnRep_IsTopDown(const bool& OldIsTopDown) const;
};
