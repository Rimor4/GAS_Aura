// Copyright Lucas Wang

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Game, meta = (DisplayName = "Top Down"))
	bool bIsTopDown = true;

protected:
	virtual void StartPlay() override;

	UFUNCTION(BlueprintCallable, Category=Game)
	void ChangeIsTopDown(const bool bNewIsTopDown);
};
