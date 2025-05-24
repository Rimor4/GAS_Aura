// Copyright Lucas Wang


#include "Game/AuraGameStateBase.h"

#include "Net/UnrealNetwork.h"


void AAuraGameStateBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(AAuraGameStateBase, bIsTopDown, COND_None, REPNOTIFY_Always);
}

void AAuraGameStateBase::OnRep_IsTopDown(const bool& OldIsTopDown) const
{
	OnIsTopDownChanged.Broadcast(bIsTopDown);
}
