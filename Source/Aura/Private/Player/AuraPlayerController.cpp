// Copyright Lucas Wang


#include "Player/AuraPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Game/AuraGameStateBase.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

#pragma region Life Cycle

class AAuraHUD;

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;
	
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

	// 绑定交互模式的变化回调
	AAuraGameStateBase* GameState = GetWorld()->GetGameState<AAuraGameStateBase>();
	if (GameState)
	{
		GameState->OnIsTopDownChanged.AddDynamic(this, &ThisClass::IsTopDownChanged);
	}
}

void AAuraPlayerController::PlayerTick(const float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (bIsTopDown)
	{
		CursorTrace();
		AutoRun();
	}
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

	// 所有的（DA格式）技能输入都绑定到这三个函数中
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed,
	                                       &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

// TODO: 重构TopDown切换
void AAuraPlayerController::IsTopDownChanged(const bool bNewIsTopDown)
{
	if (bNewIsTopDown == bIsTopDown) return;

	// TODO:切换IMC
	if (bNewIsTopDown)
	{
		// // 进入上帝视角
		// SetViewTargetWithBlend(this, 0.5f);
	}
	else
	{
		// // 进入第一人称视角
		// SetViewTargetWithBlend(GetPawn(), 0.5f);
	}
	
	bIsTopDown = bNewIsTopDown;
}

void AAuraPlayerController::CursorTrace()
{
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

void AAuraPlayerController::AbilityInputTagPressed(const FGameplayTag InputTag)
{
	if (bIsTopDown)
	{
		// 鼠标左键按下
		if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
		{
			bTargeting = ThisActor != nullptr;

			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::AbilityInputTagReleased(const FGameplayTag InputTag)
{
	if (bIsTopDown)
	{
		// TODO: 走A.
		
		// 无论按键，都调Release
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}

		// 鼠标左键短按松开（没有瞄准目标时）生成寻路路径
		if (!bTargeting && InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
		{
			if (const APawn* ControlledPawn = GetPawn(); ControlledPawn && FollowTime <= ShortPressThreshold)
			{
				BuildAutoRunPathToTarget();
			}

			FollowTime = 0.f;
		}
	}
}

void AAuraPlayerController::AbilityInputTagHeld(const FGameplayTag InputTag)
{
	if (bIsTopDown)
	{
		if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB) or bTargeting)
		{
			// 非"鼠标左键瞄准敌人"时，触发技能
			if (GetASC())
			{
				GetASC()->AbilityInputTagHeld(InputTag);
			}
			return;
		}

		// 鼠标左键长按移动状态
		FollowTime += GetWorld()->GetDeltaSeconds();

		if (CursorHit.bBlockingHit)
		{
			CachedDestination = CursorHit.ImpactPoint;
		}

		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

#pragma endregion


#pragma region Movement

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

void AAuraPlayerController::BuildAutoRunPathToTarget()
{
	if (const auto ControlledPawn = GetPawn<APawn>(); !ControlledPawn) return;

	const FVector PawnLocation = GetPawn()->GetActorLocation();
	// 尝试将点击位置投影到导航网格
	FNavLocation ProjectedLocation;
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	bool bIsOnNavMesh = NavSys->ProjectPointToNavigation(CachedDestination, ProjectedLocation, FVector::ZeroVector,
	                                                     static_cast<const FNavAgentProperties*>(nullptr));

	if (!bIsOnNavMesh)
	{
		// 如果不在导航网格上，尝试选取网格上离目标位置最近的点
		// todo：这里假设都在一个平面
		const FVector FootLocation = FVector(PawnLocation.X, PawnLocation.Y, CachedDestination.Z);

		// 从脚部位置向目标点寻找阻塞点
		FVector HitLocation;
		if (NavSys->NavigationRaycast(GetWorld(), FootLocation, CachedDestination, HitLocation))
		{
			CachedDestination = HitLocation;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("导航算法有误，未找到目标点"));
			return;
		}
	}

	// 使用修正后的目标点进行路径查找
	if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
		this, PawnLocation, CachedDestination))
	{
		Spline->ClearSplinePoints();
		for (const FVector& PointLoc : NavPath->PathPoints)
		{
			Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
#if WITH_EDITOR
			DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Green, false, 5.f);
#endif
		}
		bAutoRunning = true;
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;

	if (APawn* ControllerPawn = GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControllerPawn->GetActorLocation(),
			ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(ControllerPawn->GetActorLocation(),
		                                                                      ESplineCoordinateSpace::World);
		ControllerPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (FVector2D(LocationOnSpline.X, LocationOnSpline.Y) - FVector2D(
			CachedDestination.X, CachedDestination.Y)).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
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

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}
