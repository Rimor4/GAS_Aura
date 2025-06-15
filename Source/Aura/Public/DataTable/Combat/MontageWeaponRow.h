#pragma once

#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "MontageWeaponRow.generated.h"

USTRUCT(BlueprintType)
struct FMontageWeaponRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag WeaponTag = FGameplayTag();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName WeaponSocketProperty;
};