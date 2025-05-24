// Copyright Lucas Wang


#include "Game/AuraGameModeBase.h"

#include "Game/AuraGameStateBase.h"

void AAuraGameModeBase::StartPlay()
{
	Super::StartPlay();
}

// Server-Only
void AAuraGameModeBase::ChangeIsTopDown(const bool bNewIsTopDown)
{
	bIsTopDown = bNewIsTopDown;
	AAuraGameStateBase* GS = GetGameState<AAuraGameStateBase>();
	if (GS)
	{
		GS->bIsTopDown = bNewIsTopDown;
		GS->OnIsTopDownChanged.Broadcast(bNewIsTopDown);
	}
}
