// Copyright Lucas Wang


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

#pragma region Life Cycle

class AAuraHUD;

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);

	// 通过EnhancedInputLocalPlayerSubsystem应用映射上下文
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	// 设置输入模式：允许玩家同时与世界和UI界面交互...
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::PlayerTick(const float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>())
	{
		InitHUD(AuraPlayerState, AuraPlayerState->GetAbilitySystemComponent(), AuraPlayerState->GetAttributeSet());
	}
}

void AAuraPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>())
	{
		InitHUD(AuraPlayerState, AuraPlayerState->GetAbilitySystemComponent(), AuraPlayerState->GetAttributeSet());
	}
}

#pragma endregion


#pragma region Input

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);

	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed,
	                                       &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	if (ThisActor != LastActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->HighlightActor();
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
}

#pragma endregion


#pragma region HUD

void AAuraPlayerController::InitHUD(APlayerState* AuraPlayerState, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(GetHUD()))
	{
		AuraHUD->InitOverlay(this, AuraPlayerState, ASC, AS);
	}
}

#pragma endregion
