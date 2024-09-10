#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ExcelImportSettings.generated.h"

/**
 * Structure for the data table used to import Excel file settings.
 */
USTRUCT(BlueprintType)
struct FExcelImportSettings : public FTableRowBase
{
    GENERATED_BODY()

public:
    // The full path to the Excel file to be imported. Include the file extension (e.g., .xlsx).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Excel Import", meta = (Tooltip = "Specify the path to the Excel file. Use a relative path from the project root folder. Example: Data/Excel/MyFile.xlsx."))
    FString ExcelFilePath;

    // The name of the sheet within the Excel file to import. If left blank, the first sheet will be used.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Excel Import", meta = (Tooltip = "Specify the name of the sheet in the Excel file. No relative path is needed; just provide the sheet name. Example: Sheet1."))
    FString SheetName;

    // The path where the generated code file will be saved. This includes the directory and the file name (e.g., /Scripts/GeneratedCode/MyGeneratedCode.h).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Excel Import", meta = (Tooltip = "Specify the path where the generated code file will be saved. Use a relative path from the project root folder. Example: Generated/Code/."))
    FString GeneratedCodePath;

    // The path where the generated DataTable asset will be saved. Include the asset name and extension (e.g., /Game/DataTables/MyDataTable).
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Excel Import", meta = (Tooltip = "Specify the path for the generated data table. Use a relative path from the project root folder. Example: Content/DataTables/MyDataTable.uasset."))
    FString DataTablePath;
};
