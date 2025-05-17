// Copyright Lucas Wang

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

struct FGameplayTag;
class UAuraInputConfig;
class UAttributeSet;
class UAbilitySystemComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActiveValue;
class IEnemyInterface;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();
	
	virtual void PlayerTick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnRep_PlayerState() override;

	void InitHUD(APlayerState* AuraPlayerState, UAbilitySystemComponent* ASC, UAttributeSet* AS);

protected:
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	/**
	 * @param InputActionValue 玩家(按下WASD键时的)输入
	 */
	void Move(const struct FInputActionValue& InputActionValue);

	/*
	 * 高亮玩家光标下的敌人
	 */
	void CursorTrace();
	
	IEnemyInterface* LastActor;
	
	IEnemyInterface* ThisActor;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;
};
