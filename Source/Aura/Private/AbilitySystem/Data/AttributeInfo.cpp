// Copyright Lucas Wang


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::GetAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	if (!AttributeInformation.Contains(AttributeTag))
	{
		if (bLogNotFound)
		{
			UE_LOG(LogTemp, Error, TEXT("Can't find Info for AttributeTag [%s] on AttributeInfo [%s]"), *AttributeTag.ToString(), *GetNameSafe(this));
		}
		return FAuraAttributeInfo();
	}
	
	return AttributeInformation[AttributeTag];
}
