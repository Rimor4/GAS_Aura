// Copyright Lucas Wang


#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"


void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const APawn* OwningPawn = AIOwner->GetPawn();
	
	const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");
	
	TArray<AActor*> ActorsWithTag;
	UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, ActorsWithTag);

	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* NearestActor = nullptr;
	for (AActor* Actor : ActorsWithTag)
	{
		if (IsValid(Actor) && IsValid(OwningPawn))
		{
			if (const float Distance = OwningPawn->GetDistanceTo(Actor); Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				NearestActor = Actor;
			}
		}
	}

	UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, NearestActor);
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, ClosestDistance);
}
