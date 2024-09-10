#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DataTypeSettings.generated.h"

/**
 * Structure for the data table used to import Excel file settings.
 */
USTRUCT(BlueprintType)
struct FDataTypeSettings : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Import Type", meta = (Tooltip = "Corresponding Unreal Engine data type for the input from the Excel file"))
    FString UnrealCodeDataType;
};
