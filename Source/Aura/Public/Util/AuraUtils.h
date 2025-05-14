#pragma once
#include "GameplayTagContainer.h"

class FAuraUtils
{
public:
	template <typename T>
	static T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
	{
		return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
	}
};
