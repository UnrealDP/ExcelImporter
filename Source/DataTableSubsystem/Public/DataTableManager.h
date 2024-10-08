// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EDataTable.h"
#include "Engine/DataTable.h"

/**
 * 데이터 테이블 관리 클래스
 * 시험적으로 싱글턴으로 만든다. 전체 게임 구조에 따라서 EnginSubsystem or GameInstance 등으로 변경한다.
 * 데이터 테이블은 보통 게임 시작시 로드하고 쭉 사용하기 때문에 따로 데이터 unload 같은건 안만들었음
 * 혹시 나중에 자주 사용하지 않는 데이터는 레퍼런스 관리 같은거 할거라면 수정이 필요함
 * unload 가 없다고 가정하고 TArray<UDataTable*> DataTableArray 로 관리함
 * 한번 로드하고 언로드 안할건데 InitializeDataTables(const TArray<EDataTable>& PreloadTables) 이런게 있는 이유는
 * 툴 같은 곳에서는 전부 로드하지 않고 필요한거만 로드할거라서
 */
class DATATABLESUBSYSTEM_API DataTableManager
{
private:
    static DataTableManager* Instance;

public:
    static bool IsInitialized()
	{
		return Instance != nullptr;
	}
    static DataTableManager& Get();
    static void Destroy();

    // 복사 또는 이동을 방지
    DataTableManager(const DataTableManager&) = delete;
    DataTableManager& operator=(const DataTableManager&) = delete;

    static UDataTable* DataTable(EDataTable TableType)
    {
        return Get().GetDataTable(TableType);
    }
    template<typename T>
    static UDataTable* DataTable()
    {
        EDataTable TableType = TypeIndex<T>::Gettype();
        return Get().GetDataTable(TableType);
    }

    template<typename T>
    static T* Data(const FName& RowKey)
    {
        return Get().GetData<T>(RowKey);
    }

    // 상속 받은 데이터의 경우 실제 데이터 enum을 넘겨주고 캐스팅은 부모로 가능한 함수
    template<typename T>
    static T* Data(EDataTable TableType, const FName& RowKey)
    {
        return Get().GetData<T>(TableType, RowKey);
    }

    template<typename T>
    static T* Data(UDataTable* InDataTable, const FName& RowKey)
    {
        return Get().GetData<T>(InDataTable, RowKey);
    }

protected:
    DataTableManager() 
    {
        DataTablePaths.SetNum(static_cast<int32>(EDataTable::Max));
        DataTableArray.SetNum(static_cast<int32>(EDataTable::Max));
    }

    ~DataTableManager()
    {
        for (UDataTable*& DataTable : DataTableArray)
        {
            if (DataTable)
            {
                DataTable = nullptr;
            }
        }
        CollectGarbage(RF_NoFlags);

        DataTablePaths.Empty();
        DataTableArray.Empty();

        bIsInitDataTablePath = false;

        UE_LOG(LogTemp, Log, TEXT("DataTableManager destroyed"));
    }

    template<typename T>
    FString GetContextString()
    {
        // 컴파일 타임에 FTableRowBase 상속 여부를 체크
        static_assert(TIsDerivedFrom<T, FTableRowBase>::IsDerived, "DataTableManager::GetContextString <T> must derive from FTableRowBase");
        return FString::Printf(TEXT("%sContext"), *T::StaticStruct()->GetName());
    }

    template<typename T>
    T* GetData(const FName& RowKey)
    {
        EDataTable TableType = TypeIndex<T>::GetType();
        if (TableType == EDataTable::Max)
        {
            UE_LOG(LogTemp, Error, TEXT("DataTableManager::GetData TableType is invalid [ %d ]"), static_cast<int32>(TableType));
            return nullptr;
        }

        return GetData<T>(TableType, RowKey);
    }

    template<typename T>
    T* GetData(UDataTable* InDataTable, const FName& RowKey)
    {
        if (!InDataTable)
        {
            UE_LOG(LogTemp, Error, TEXT("DataTableManager::GetData InDataTable is invalid"));
            return nullptr;
        }

        FString ContextString = GetContextString<T>();
        return InDataTable->FindRow<T>(RowKey, ContextString, true);
    }

    template<typename T>
    T* GetData(EDataTable TableType, const FName& RowKey)
    {
        UDataTable* DataTable = GetDataTable(TableType);
        if (!DataTable)
        {
            UE_LOG(LogTemp, Error, TEXT("DataTableManager::GetData DataTable is invalid %d"), static_cast<int32>(TableType));
            return nullptr;
        }

        return GetData<T>(DataTable, RowKey);
    }

private:

    // 설정 테이블에서 DataTable 경로를 가져오는 함수
    FString GetDataTablePath(EDataTable TableType);

    // 실제 데이터 테이블을 로드하는 함수
    UDataTable* LoadDataTable(EDataTable TableType);

    // 파일명에서 enum 값을 추출하는 함수
    EDataTable StringToEDataTable(const FString& EnumString);

public:

    // 데이터 테이블 경로를 저장하는 배열 초기화 함수
    void InitDataTablePath();

    // 데이터 테이블 초기화 함수 (미리 로드할 enum 리스트를 받음)
    void InitializeDataTables(const TArray<EDataTable>& PreloadTables);
    void ReleaseDatas(const TArray<EDataTable>& PreloadTables);

    UDataTable* GetDataTable(EDataTable TableType);

    // 로드된 모든 데이터 테이블 리셋 (DataTablePaths, DataTableArray 의 Array 버퍼는 살아 있음 내용만 날아감)
    void ResetLoadedDataTables();

private:
    // 데이터 테이블 경로를 저장하는 배열 초기화 유무
    bool bIsInitDataTablePath = false;

    // 데이터 테이블 경로를 저장하는 배열
    TArray<FString> DataTablePaths;

    // 로드된 데이터 테이블 배열
    TArray<UDataTable*> DataTableArray;
};
