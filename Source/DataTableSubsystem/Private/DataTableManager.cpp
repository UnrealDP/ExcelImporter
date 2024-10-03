// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableManager.h"
#include "ExcelImportSettings.h"
#include "Misc/PathsUtil.h"

DataTableManager* DataTableManager::Instance = nullptr;

DataTableManager& DataTableManager::Get()
{
    if (!Instance)
    {
        Instance = new DataTableManager();
    }
    return *Instance;
}

void DataTableManager::Destroy()
{
    if (Instance)
    {
        bool bIsInitDataTablePath = false;

        for (UDataTable*& data : Instance->DataTableArray)
        {
            data->RemoveFromRoot();
            data = nullptr;
        }

        Instance->DataTablePaths.Empty();
        Instance->DataTableArray.Empty();

        delete Instance;
        Instance = nullptr;

        CollectGarbage(RF_NoFlags);
    }
}

// 데이터 테이블 경로를 저장하는 배열 초기화 함수
void DataTableManager::InitDataTablePath()
{
    if (bIsInitDataTablePath)
    {
		return;
	}

    // 설정 파일을 통해 경로 정보를 로드
    UDataTable* ExcelImportSettingsDataTable = LoadObject<UDataTable>(nullptr, TEXT("/ExcelImporter/DT_ExcelImportSettings"));
    if (ExcelImportSettingsDataTable)
    {
        TArray<FExcelImportSettings*> Rows;
        ExcelImportSettingsDataTable->GetAllRows(TEXT("ExcelImportSettingsContext"), Rows);

        // 각 경로에 해당하는 데이터 테이블 경로를 저장
        for (FExcelImportSettings* Row : Rows)
        {
            if (!Row)
            {
                continue;
            }

            if (Row->DataTablePath.IsEmpty())
            {
                UE_LOG(LogTemp, Error, TEXT("DataTableManager::InitializeDataTables DataTablePath is empty"));
                continue;
            }

            if (Row->GeneratedCodePath.IsEmpty())
            {
                UE_LOG(LogTemp, Error, TEXT("DataTableManager::InitializeDataTables DataTablePath is empty"));
                continue;
            }

            FString BaseFileName = FPaths::GetBaseFilename(Row->GeneratedCodePath);
            EDataTable TableType = StringToEDataTable(BaseFileName);
            if (TableType == EDataTable::Max)
			{
				UE_LOG(LogTemp, Error, TEXT("DataTableManager::InitializeDataTables Invalid TableType: %s"), *BaseFileName);
				continue;
			}

            FString ResPath = PathsUtil::GetGameAssetPathFromFilePath(FString::Printf(TEXT("%s/DT_%s"), *Row->DataTablePath, *BaseFileName));
            DataTablePaths[static_cast<int32>(TableType)] = ResPath;
        }

        bIsInitDataTablePath = true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load settings data table: %s"), TEXT("/ExcelImporter/DT_ExcelImportSettings"));
    }
}

// 데이터 테이블 초기화 함수 (미리 로드할 enum 리스트를 받음)
void DataTableManager::InitializeDataTables(const TArray<EDataTable>& PreloadTables)
{
    // 데이터 테이블 경로를 저장하는 배열 초기화
    InitDataTablePath();

    // PreloadTables 배열에 있는 enum에 해당하는 테이블 미리 로드
    for (EDataTable TableType : PreloadTables)
    {
        if (DataTableArray[static_cast<int32>(TableType)] == nullptr)
        {
            LoadDataTable(TableType);
        }
    }
}

// 데이터 테이블 로드한 것 릴리즈 하는 함수
void DataTableManager::ReleaseDatas(const TArray<EDataTable>& PreloadTables)
{
    for (EDataTable TableType : PreloadTables)
    {
        if (DataTableArray[static_cast<int32>(TableType)])
        {
            DataTableArray[static_cast<int32>(TableType)]->RemoveFromRoot();
            DataTableArray[static_cast<int32>(TableType)] = nullptr;
        }
    }
    CollectGarbage(RF_NoFlags);
}

// 설정 테이블에서 DataTable 경로를 가져오는 함수
FString DataTableManager::GetDataTablePath(EDataTable TableType)
{
    int32 TableIndex = static_cast<int32>(TableType);
    if (TableIndex < DataTablePaths.Num())
    {
        return DataTablePaths[TableIndex];
    }

    return FString();  // 기본값 반환
}

// 실제 데이터 테이블을 로드하는 함수
UDataTable* DataTableManager::LoadDataTable(EDataTable TableType)
{
    if (TableType == EDataTable::Max)
    {
        UE_LOG(LogTemp, Log, TEXT("ataTableManager::LoadDataTable( TableType %d"), static_cast<int32>(TableType));
        return nullptr;
    }

    const FString DataTablePath = GetDataTablePath(TableType);
    if (DataTablePath.IsEmpty())
	{
		return nullptr;
	}

    UDataTable* LoadedTable = LoadObject<UDataTable>(nullptr, *DataTablePath);
    if (LoadedTable)
    {
        LoadedTable->AddToRoot();
        DataTableArray[static_cast<int32>(TableType)] = nullptr;
        DataTableArray[static_cast<int32>(TableType)] = LoadedTable;
        UE_LOG(LogTemp, Log, TEXT("ataTableManager::LoadDataTable( Loaded DataTable: %s"), *DataTablePath);
    }
    else
    {
		UE_LOG(LogTemp, Error, TEXT("ataTableManager::LoadDataTable( Failed to load DataTable: %s"), *DataTablePath);
	}

    return LoadedTable;
}

// 파일명에서 enum 값을 추출하는 함수
EDataTable DataTableManager::StringToEDataTable(const FString& EnumString)
{
    // StaticEnum을 사용해 EDataTable enum의 UEnum 객체를 가져옴
    UEnum* EnumPtr = StaticEnum<EDataTable>();
    if (!EnumPtr)
    {
        return EDataTable::Max;
    }

    // Enum의 이름을 FName으로 변환하고 enum 값 반환
    int32 EnumValue = EnumPtr->GetValueByName(FName(*EnumString));
    if (EnumValue == INDEX_NONE)
    {
        return EDataTable::Max;
    }

    return static_cast<EDataTable>(EnumValue);
}

// 배열에서 데이터 테이블을 반환하는 함수 (로드되지 않았으면 로드)
UDataTable* DataTableManager::GetDataTable(EDataTable TableType)
{
    if (!bIsInitDataTablePath)
    {
        InitDataTablePath();
    }

    // 요청된 데이터 테이블이 배열에 없다면 로드
    if (DataTableArray[static_cast<int32>(TableType)] == nullptr)
    {
        LoadDataTable(TableType);
    }

    return DataTableArray[static_cast<int32>(TableType)];
}

void DataTableManager::ResetLoadedDataTables()
{
    DataTablePaths.Reset();

    for (UDataTable*& DataTable : DataTableArray)
    {
        if (DataTable)
        {
            DataTable->RemoveFromRoot();
            DataTable = nullptr;
        }
    }
    CollectGarbage(RF_NoFlags);
    DataTableArray.Reset();

    bIsInitDataTablePath = false;
}
