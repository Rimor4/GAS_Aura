#pragma once
#include "Game/AuraGameInstanceBase.h"

class FAuraUtils
{
public:
	template<typename T>
	static T* GetDataTableRow(const UObject* WorldContextObject, const FName TableName, const FName RowName)
	{
		return Cast<UAuraGameInstanceBase>(WorldContextObject->GetWorld()->GetGameInstance())->GetDataTableRow<T>(TableName, RowName);
	}

	static FName GetFNamePropertyValue(const UObject* Object, const FName PropertyName)
	{
		const UClass* Class = Object->GetClass();
		if (const FProperty* Property = Class->FindPropertyByName(PropertyName))
		{
			if (const FNameProperty* NameProperty = CastField<FNameProperty>(Property))
			{
				return NameProperty->GetPropertyValue_InContainer(Object);
			}
		}
		return NAME_None;
	}
};
