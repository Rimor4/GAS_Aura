// Copyright Lucas Wang

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

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

	void InitHUD(APlayerController* AuraPlayerController, APlayerState* AuraPlayerState,
	             UAbilitySystemComponent* ASC, UAttributeSet* AS) const;

protected:
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	void Move(const struct FInputActionValue& InputActionValue);

	void CursorTrace();
	
	TObjectPtr<IEnemyInterface> LastActor;
	
	TObjectPtr<IEnemyInterface> ThisActor;
};
