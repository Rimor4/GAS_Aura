// Copyright Lucas Wang

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstanceBase.generated.h"

class UDataTableManager;
/**
 * 
 */
UCLASS()
class AURA_API UAuraGameInstanceBase : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;

#pragma region Data Table
	UDataTable* GetDataTable(const FName TableName)
	{
		if (DataTables.Contains(TableName))
		{
			return DataTables[TableName];
		}
		return nullptr;
	}

	template<typename T>
	T* GetDataTableRow(const FName TableName, FName RowName)
	{
		if (const UDataTable* Table = GetDataTable(TableName))
		{
			return Table->FindRow<T>(RowName, "");
		}
		return nullptr;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data Tables")
	TMap<FName, UDataTable*> DataTables;
#pragma endregion 
	
};
