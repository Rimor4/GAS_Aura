// Copyright Lucas Wang

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class USplineComponent;
class UAuraAbilitySystemComponent;
struct FGameplayTag;
class UAuraInputConfig;
class UAttributeSet;
class UAbilitySystemComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActiveValue;
class IEnemyInterface;
class AAuraGameStateBase;

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
#pragma region Input
	bool bIsTopDown = true;

	UFUNCTION()
	void IsTopDownChanged(const bool bNewIsTopDown);
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	void AbilityInputTagPressed(const FGameplayTag InputTag);
	void AbilityInputTagReleased(const FGameplayTag InputTag);
	void AbilityInputTagHeld(const FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;
#pragma endregion


	/**
	 * @param InputActionValue 玩家(按下WASD键时的)输入
	 */
	void Move(const struct FInputActionValue& InputActionValue);


#pragma region Cursor
	/*
	 * 高亮玩家光标下的敌人
	 */
	void CursorTrace();

	IEnemyInterface* LastActor = nullptr;
	IEnemyInterface* ThisActor = nullptr;

	FHitResult CursorHit;

	// 玩家光标是否正在瞄准某个物体
	bool bTargeting = false;
#pragma endregion


#pragma region Click to Move
	FVector CachedDestination = FVector::ZeroVector;

	float FollowTime = 0.f;

	float ShortPressThreshold = 0.5f;

	// 玩家是否将要在鼠标点击地面后自动移动
	bool bAutoRunning = false;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	void BuildAutoRunPathToTarget();
	
	void AutoRun();
#pragma endregion


	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetASC();
};
