#include "ExcelImporterWidget.h"
#include "ExcelImportSettings.h"
#include "SlateOptMacros.h"
#include "SlateEditorUtils.h"
#include "EditorPackageUtils.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/SCompoundWidget.h"
#include "OpenXLSX.hpp"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Misc/MessageDialog.h"
#include "DataTypeSettings.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Interfaces/IPluginManager.h"
#include "PackageTools.h"
#include "ObjectTools.h"
 
void SExcelImporterWidget::Construct(const FArguments& InArgs)
{
    LoadData();
    LoadExcelFiles();

    ChildSlot
        [
            SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(STextBlock).Text(FText::FromString(TEXT("Select Excel Files to Controll")))
                ]                
                
                + SVerticalBox::Slot()  // SVerticalBox 안에 SHorizontalBox를 포함
                .AutoHeight()
                [
                    SNew(SHorizontalBox)  // SHorizontalBox로 가로 정렬 시작
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SlateEditorUtils::CreateButtonWithText(FText::FromString(TEXT("Select All")),
                                FOnClicked::CreateSP(this, &SExcelImporterWidget::OnSelectAllButtonClicked))  // Select All 버튼 클릭 함수
                        ]

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SlateEditorUtils::CreateButtonWithText(FText::FromString(TEXT("UnSelect All")),
                                FOnClicked::CreateSP(this, &SExcelImporterWidget::OnUnSelectAllButtonClicked))  // UnSelect All 버튼 클릭 함수
                        ]
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SAssignNew(ExcelListView, SListView<TSharedPtr<FExcelFileItem>>)  // SListView 인스턴스 생성 및 초기화
                        .ListItemsSource(&ExcelFiles)
                        .OnGenerateRow(this, &SExcelImporterWidget::GenerateFileRow)
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(STextBlock)
                        .Text(ErrorMessage)
                        .ColorAndOpacity(FLinearColor::Red)
                        .Visibility(this, &SExcelImporterWidget::GetErrorMessageVisibility)
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SlateEditorUtils::CreateButtonWithText(FText::FromString(TEXT("Generated Selected to C++ Header")),
                        FOnClicked::CreateSP(this, &SExcelImporterWidget::OnGeneratedClicked))
                ]

                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SlateEditorUtils::CreateButtonWithText(FText::FromString(TEXT("Create DataTable")),
                        FOnClicked::CreateSP(this, &SExcelImporterWidget::OnCreateDataTableClicked))
                ]
        ];
}

EVisibility SExcelImporterWidget::GetErrorMessageVisibility() const
{
    return ErrorMessage.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

void SExcelImporterWidget::LoadData()
{
    FString ExcelImportSettingsPath = TEXT("/ExcelImporter/DT_ExcelImportSettings");
    ExcelImportSettingsDataTable = LoadObject<UDataTable>(nullptr, *ExcelImportSettingsPath);

    FString DataTypeSettingsPath = TEXT("/ExcelImporter/DT_DataTypeSettings");
    DataTypeSettingsDataTable = LoadObject<UDataTable>(nullptr, *DataTypeSettingsPath);
}

// ExcelImportSettingsDataTable 에 입력한 액셀 파일 있는지 확인하고 액셀 리스트를 뽑음
void SExcelImporterWidget::LoadExcelFiles()
{
    if (ExcelImportSettingsDataTable == nullptr)
    {
        ErrorMessage = FText::FromString(TEXT("Failed to load the settings file. (ExcelImportSettingsDataTable)"));
        return;
    }

    if (DataTypeSettingsDataTable == nullptr)
    {
        ErrorMessage = FText::FromString(TEXT("Failed to load the settings file. (DataTypeSettingsDataTable)"));
        return;
    }

    FString ErrFileLog = "";
    TArray<FExcelImportSettings*> AllRows;
    ExcelImportSettingsDataTable->GetAllRows<FExcelImportSettings>(TEXT("LoadExcelFiles"), AllRows);
    ExcelFiles.Empty();

    bool bFilesFound = true;
    for (const FExcelImportSettings* Row : AllRows)
    {
        if (Row)
        {
            FString FullFilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() / Row->ExcelFilePath);

            if (IFileManager::Get().FileExists(*FullFilePath))
            {
                TSharedPtr<FExcelFileItem> NewItem = MakeShareable(new FExcelFileItem(*Row));
                ExcelFiles.Add(NewItem);
            }
            else
            {
                bFilesFound = false;
                ErrFileLog += (FullFilePath + "\n");
            }
        }
    }

    if (!bFilesFound)
    {
        ErrFileLog = "Files that don't exist\n" + ErrFileLog;
        ErrorMessage = FText::FromString(ErrFileLog);
    }
    else
    {
        ErrorMessage = FText::FromString(TEXT(""));
    }
}

TSharedRef<ITableRow> SExcelImporterWidget::GenerateFileRow(TSharedPtr<FExcelFileItem> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
    return SNew(STableRow<TSharedPtr<FExcelFileItem>>, OwnerTable)
        [
            SNew(SHorizontalBox)

                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SCheckBox)
                        .IsChecked(this, &SExcelImporterWidget::GetCheckBoxState, InItem)  // 체크 상태 처리 함수
                        .OnCheckStateChanged(this, &SExcelImporterWidget::OnCheckBoxStateChanged, InItem)  // 체크 상태 변경 함수
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(InItem->GetFullExcelFilePath()))  // 파일 이름을 표시
                ]
        ];
}

ECheckBoxState SExcelImporterWidget::GetCheckBoxState(TSharedPtr<FExcelFileItem> InItem) const
{
    return InItem->bIsChecked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SExcelImporterWidget::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FExcelFileItem> InItem)
{
    InItem->bIsChecked = (NewState == ECheckBoxState::Checked);
}


void SExcelImporterWidget::OnFileCheckboxChanged(ECheckBoxState NewState, TSharedPtr<FString> FileName)
{
}

FReply SExcelImporterWidget::OnGeneratedClicked()
{
    // 체크된 파일 목록
    TArray<TSharedPtr<FExcelFileItem>> FilesToConvert;
    for (const TSharedPtr<FExcelFileItem>& Item : ExcelFiles)
    {
        if (Item->bIsChecked)
        {
            FilesToConvert.Add(Item);
        }
    }

    if (FilesToConvert.Num() == 0)
    {
        ErrorMessage = FText::FromString(TEXT("No files selected for conversion."));
        return FReply::Handled();
    }

    TArray<FString> ExcelPaths;
    TArray<FString> SheetNames;
    TArray<FString> GeneratedCodePaths;

    for (const TSharedPtr<FExcelFileItem>& Item : FilesToConvert)
    {
        FString ExcelFilePath = Item->GetFullExcelFilePath();  // Excel 파일 경로 가져오기
        FString SheetName = Item->GetSheetName();  // 시트명 가져오기
        FString GeneratedCodePath = Item->GetFullGeneratedCodePath();  // 코드 출력 경로 가져오기

        // 각각의 배열에 추가
        ExcelPaths.Add(ExcelFilePath);
        SheetNames.Add(SheetName);
        GeneratedCodePaths.Add(GeneratedCodePath);
    }

    // 이제 ExcelPaths와 OutputPaths를 ConvertMultipleExcelToCPP 함수로 넘길 수 있음
    if (ConvertMultipleExcelToCPP(ExcelPaths, SheetNames, GeneratedCodePaths))
    {
        //UE_LOG(LogTemp, Log, TEXT("Excel to CPP 변환 성공"));
    }
    else
    {
        //UE_LOG(LogTemp, Error, TEXT("Excel to CPP 변환 실패"));
    }

    return FReply::Handled();
}

FReply SExcelImporterWidget::OnCreateDataTableClicked()
{
    // 체크된 파일 목록
    TArray<TSharedPtr<FExcelFileItem>> FilesToConvert;
    for (const TSharedPtr<FExcelFileItem>& Item : ExcelFiles)
    {
        if (Item->bIsChecked)
        {
            FilesToConvert.Add(Item);
        }
    }

    if (FilesToConvert.Num() == 0)
    {
        ErrorMessage = FText::FromString(TEXT("No files selected for conversion."));
        return FReply::Handled();
    }

    TArray<FString> ExcelPaths;
    TArray<FString> SheetNames;
    TArray<FString> GeneratedCodePaths;
    TArray<FString> DataTablePaths;

    for (const TSharedPtr<FExcelFileItem>& Item : FilesToConvert)
    {
        FString ExcelFilePath = Item->GetFullExcelFilePath();  // Excel 파일 경로 가져오기
        FString SheetName = Item->GetSheetName();  // 시트명 가져오기
        FString GeneratedCodePath = Item->GetFullGeneratedCodePath();  // 코드 출력 경로 가져오기
        FString DataTablePath = Item->GetFullDataTablePath();  // DataTable 생성 경로 가져오기

        // 각각의 배열에 추가
        ExcelPaths.Add(ExcelFilePath);
        SheetNames.Add(SheetName);
        GeneratedCodePaths.Add(GeneratedCodePath);
        DataTablePaths.Add(DataTablePath);
    }

    if (CreateDataTable(ExcelPaths, SheetNames, GeneratedCodePaths, DataTablePaths))
    {
        //UE_LOG(LogTemp, Log, TEXT("Excel to CPP 변환 성공"));
    }
    else
    {
        //UE_LOG(LogTemp, Error, TEXT("Excel to CPP 변환 실패"));
    }

    return FReply::Handled();
}

FReply SExcelImporterWidget::OnSelectAllButtonClicked()
{
    // 모든 파일의 체크 상태를 선택
    for (TSharedPtr<FExcelFileItem>& Item : ExcelFiles)
    {
        Item->bIsChecked = true;  // 모든 파일을 선택 상태로 설정
    }

    ExcelListView->RequestListRefresh();  // 리스트 뷰를 갱신
    return FReply::Handled();  // 버튼 클릭을 처리했다고 반환
}

FReply SExcelImporterWidget::OnUnSelectAllButtonClicked()
{
    // 모든 파일의 체크 상태를 해제
    for (TSharedPtr<FExcelFileItem>& Item : ExcelFiles)
    {
        Item->bIsChecked = false;  // 모든 파일을 선택 해제 상태로 설정
    }

    ExcelListView->RequestListRefresh();  // 리스트 뷰를 갱신
    return FReply::Handled();  // 버튼 클릭을 처리했다고 반환
}

// Excel 파일을 읽고 C++ 코드 생성
bool SExcelImporterWidget::ConvertMultipleExcelToCPP(const TArray<FString>& ExcelPaths, const TArray<FString>& SheetNames, const TArray<FString>& GeneratedCodePaths)
{
    if (ExcelPaths.Num() != SheetNames.Num())
    {
        // Excel 경로와 출력 경로의 개수가 일치하지 않습니다.
        UE_LOG(LogTemp, Error, TEXT("The number of Excel paths does not match the number of output paths."));
        return false;
    }

    TArray<FString> SuccessFilesList;
    TArray<FString> FailureFilesList;

    for (int32 i = 0; i < ExcelPaths.Num(); ++i)
    {
        FString ExcelFilePath = ExcelPaths[i];
        FString SheetName = SheetNames[i];
        FString OutputCodePath = GeneratedCodePaths[i];

        try
        {
            // OpenXLSX로 Excel 파일 열기
            OpenXLSX::XLDocument doc;
            doc.open(TCHAR_TO_UTF8(*ExcelFilePath));

            OpenXLSX::XLWorksheet wks = doc.workbook().worksheet(TCHAR_TO_UTF8(*SheetName));

            // 첫 번째와 두 번째 행 읽기
            TArray<FString> FirstRow;
            TArray<FString> SecondRow;

            int row = 1;
            auto rowIter = wks.rows().begin();

            // 첫 번째 행 (변수명)
            for (auto cell : rowIter->cells())
            {
                FirstRow.Add(UTF8_TO_TCHAR(cell.value().getString().c_str()));
            }

            ++rowIter;
            // 두 번째 행 (자료형)
            for (auto cell : rowIter->cells())
            {
                SecondRow.Add(UTF8_TO_TCHAR(cell.value().getString().c_str()));
            }

            if (FirstRow.Num() != SecondRow.Num())
            {
                FailureFilesList.Add(ExcelFilePath);
                continue;
            }

            // C++ 코드 생성
            FString BaseFileName = FPaths::GetBaseFilename(ExcelFilePath);
            FString GeneratedCode = GenerateCPPCode(BaseFileName, FirstRow, SecondRow);
            FFileHelper::SaveStringToFile(GeneratedCode, *OutputCodePath);

            SuccessFilesList.Add(ExcelFilePath);
            doc.close();
        }
        catch (const std::exception& ex)
        {
            FailureFilesList.Add(ExcelFilePath);
            // "Excel 파일 처리 중 오류 발생: %s"
            UE_LOG(LogTemp, Error, TEXT("An error occurred while processing the Excel file."), *FString(ex.what()));
        }
    }

    FText SuccessFilesText;
    FText FailureFilesText;

    if (SuccessFilesList.Num() == 0)
    {
        SuccessFilesText = FText::FromString(TEXT("None"));
    }
    else
    {
        SuccessFilesText = FText::FromString(FString::Join(SuccessFilesList, TEXT("\n")));
    }

    if (FailureFilesList.Num() == 0)
    {
        FailureFilesText = FText::FromString(TEXT("None"));
    }
    else
    {
        FailureFilesText = FText::FromString(FString::Join(FailureFilesList, TEXT("\n")));
    }

    // 최종 메시지 생성 -> "성공한 파일:\n{0}\n실패한 파일:\n{1}"
    FText ResultMessage = FText::Format(FText::FromString(TEXT("Successful files:\n{0}\nFailed files:\n{1}")), SuccessFilesText, FailureFilesText);

    // 결과를 팝업으로 출력
    FMessageDialog::Open(EAppMsgType::Ok, ResultMessage);

    // 사용자에게 빌드를 할지 물어보는 팝업 띄우기 (OK, Cancel 버튼 포함)
    // "헤더 파일이 생성되었습니다. 빌드를 다시 하시겠습니까?"
    EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::OkCancel, FText::FromString(TEXT("The header file has been generated. Would you like to rebuild?")));

    if (Result == EAppReturnType::Ok)
    {
        EditorPackageUtils::StartBuildAndRestartEditor();
    }
    else
    {
        // 사용자가 빌드를 취소한 경우 처리
        // 수동 빌드가 필요 합니다.\n빌드 하지 않으면 추가된 데이터는 사용이 불가능합니다
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Manual build is required.\nIf you do not rebuild, the added data will not be usable.")));
    }

    return FailureFilesList.Num() == 0;
}

// C++ 코드 생성 함수
FString SExcelImporterWidget::GenerateCPPCode(const FString& FileName, const TArray<FString>& VariableNames, const TArray<FString>& DataTypes)
{
    FString GeneratedCode = "#pragma once\n\n";
    GeneratedCode += "#include \"CoreMinimal.h\"\n";    
    GeneratedCode += "#include \"Engine/DataTable.h\"\n";
    GeneratedCode += FString::Printf(TEXT("#include \"%s.generated.h\"\n\n"), *FileName);
    GeneratedCode += "USTRUCT(BlueprintType)\n";
    GeneratedCode += FString::Printf(TEXT("struct F%s  : public FTableRowBase\n"), *FileName);
    GeneratedCode += "{\n";
    GeneratedCode += "\tGENERATED_BODY()\n\n";
    GeneratedCode += "public:\n";

    // 첫번째는 데이터 ID
    for (int i = 1; i < VariableNames.Num(); i++)
    {
        FString UnrealType = MapExcelTypeToUnreal(FName(DataTypes[i]));
        GeneratedCode += FString::Printf(TEXT("\tUPROPERTY(EditAnywhere, BlueprintReadWrite, Category = \"Data\")\n\t%s %s;\n\n"),
            *UnrealType, *VariableNames[i]);
    }

    GeneratedCode += "};\n";
    return GeneratedCode;
}

// Excel 자료형을 Unreal 자료형으로 변환하는 함수
FString SExcelImporterWidget::MapExcelTypeToUnreal(const FName& ExcelType)
{
    FString ContextString(TEXT("DataTypeSettingsContext"));
    FDataTypeSettings* Row = DataTypeSettingsDataTable->FindRow<FDataTypeSettings>(ExcelType, ContextString);
    if (Row)
    {
        return Row->UnrealCodeDataType;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not find row with key: %s in context: %s"), *ExcelType.ToString(), *ContextString);
        return TEXT("UnknownType");
    }
}

bool SExcelImporterWidget::CreateDataTable(const TArray<FString>& ExcelPaths, const TArray<FString>& SheetNames, TArray<FString>& GeneratedCodePaths, const TArray<FString>& DataTablePaths)
{
    if (ExcelPaths.Num() != DataTablePaths.Num())
    {
        // Excel 경로와 데이터 생성 경로의 개수가 일치하지 않습니다.
        UE_LOG(LogTemp, Error, TEXT("The number of Excel paths and data generation paths does not match.."));
        return false;
    }

    TArray<FString> SuccessFilesList;
    TArray<FString> FailureFilesList;

    for (int32 i = 0; i < ExcelPaths.Num(); ++i)
    {
        FString ExcelFilePath = ExcelPaths[i];
        FString SheetName = SheetNames[i];
        FString GeneratedCodePath = GeneratedCodePaths[i];
        FString DataTablePath = DataTablePaths[i];

        try
        {
            ///////////////////////////////
            // ExcelFilePath 액셀의 SheetName를 로드해서 OpenXLSX::XLWorksheet wks 를 만들었으니
            // 3번째 행부터 첫번째 열이 빈공간 일때까지 데이터를 만들어서 DataTable 로 만듬
            // 이때 언리얼 TataTable 어셋으로 만들기 위해 참고해야 할 FTableRowBase 해더 파일은 GeneratedCodePath 이 경로에 있는 해더파일
            // TataTable을 만들고 나서 DataTablePath 여기 지정한 경로에 DataTable을 생성
            // 생성하는 DataTable의 어셋명의 룰은 : DT_클래스명
            // 클래스명은 GeneratedCodePath 여기로 부터 순수 파일명을 추출한다. FString BaseClassName = FPaths::GetBaseFilename(GeneratedCodePath);
            ////////////////////////////////

            if (CreateDataTableFromExcel(ExcelFilePath, SheetName, GeneratedCodePath, DataTablePath))
            {
                SuccessFilesList.Add(ExcelFilePath);
            }
            else
            {
                FailureFilesList.Add(ExcelFilePath);
            }

        }
        catch (const std::exception& ex)
        {
            FailureFilesList.Add(ExcelFilePath);
            // "Excel 파일 처리 중 오류 발생: %s"
            UE_LOG(LogTemp, Error, TEXT("An error occurred while processing the Excel file."), *FString(ex.what()));
        }
    }

    FText SuccessFilesText;
    FText FailureFilesText;

    if (SuccessFilesList.Num() == 0)
    {
        SuccessFilesText = FText::FromString(TEXT("None"));
    }
    else
    {
        SuccessFilesText = FText::FromString(FString::Join(SuccessFilesList, TEXT("\n")));
    }

    if (FailureFilesList.Num() == 0)
    {
        FailureFilesText = FText::FromString(TEXT("None"));
    }
    else
    {
        FailureFilesText = FText::FromString(FString::Join(FailureFilesList, TEXT("\n")));
    }

    // 최종 메시지 생성 -> "성공한 파일:\n{0}\n실패한 파일:\n{1}"
    FText ResultMessage = FText::Format(FText::FromString(TEXT("Successful files:\n{0}\nFailed files:\n{1}")), SuccessFilesText, FailureFilesText);

    // 결과를 팝업으로 출력
    FMessageDialog::Open(EAppMsgType::Ok, ResultMessage);

    return FailureFilesList.Num() == 0;
}

bool SExcelImporterWidget::CreateDataTableFromExcel(const FString& ExcelFilePath, const FString& SheetName, const FString& GeneratedCodePath, const FString& DataTablePath)
{
    try
    {
        // 1. OpenXLSX로 Excel 파일 열기
        OpenXLSX::XLDocument doc;
        doc.open(TCHAR_TO_UTF8(*ExcelFilePath));

        OpenXLSX::XLWorksheet wks = doc.workbook().worksheet(TCHAR_TO_UTF8(*SheetName));

        // 2. 클래스명 추출
        FString BaseClassName = FPaths::GetBaseFilename(GeneratedCodePath); // 파일명에서 확장자 제거
        FString HeaderFilePath = FPaths::ChangeExtension(GeneratedCodePath, TEXT(".h")); // 헤더 파일 경로
        if (!FPaths::FileExists(HeaderFilePath))
        {
            UE_LOG(LogTemp, Error, TEXT("Header file not found: %s"), *HeaderFilePath);
            doc.close();
            return false;
        }

        // 3. 두 번째 행 읽어서 자료형 가져오기
        //TArray<FString> DataTypes;
        //OpenXLSX::XLRow row = wks.row(2);  // 2번째 행에 직접 접근
        //for (auto cell : row.cells())
        //{
        //    DataTypes.Add(UTF8_TO_TCHAR(cell.value().getString().c_str()));
        //}

        // 4. 해더파일 경로로 해더파일에 선언된 데이터 구조체의 구성 정보를 가져옴
        UScriptStruct* RowStruct = LoadStructFromHeaderPath(HeaderFilePath);

        // 5. DataTable 생성
        UDataTable* NewDataTable = CreateNewOrClearDataTable(RowStruct, BaseClassName, DataTablePath);
        if (!NewDataTable)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create DataTable for class: %s"), *BaseClassName);
            doc.close();
            return false;
        }

        // 6. 엑셀 데이터 3번째 행부터 파싱
        int32 RowIndex = 3; // 3번째 행부터 시작 (1번째는 데이터 변수명, 2번째는 데이터 자료형, 3번째 부터 데이터)
        while (true)
        {
            // 첫 번째 열이 빈 공간이면 반복 종료
            FString FirstColumnValue = FString(wks.cell(RowIndex, 1).value().getString().c_str());
            if (FirstColumnValue.IsEmpty())
            {
                break;
            }

            // 새 행을 DataTable에 추가
            FTableRowBase* NewRow = CreateDataTableRowFromExcel(RowStruct, wks, RowIndex);
            if (NewRow == nullptr)
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to create row for row index: %d"), RowIndex);
                doc.close();
                return false;
            }

            // DataTable에 새로 생성한 행 추가
            NewDataTable->AddRow(FName(*FirstColumnValue), *NewRow);

            RowIndex++;
        }

        // 7. DataTable 어셋 저장
        //SaveDataTableAsset(NewDataTable, DataTablePath);
        UPackage* SavedPackage = EditorPackageUtils::SaveAssetToPackage(
            NewDataTable, DataTablePath, 
            *NewDataTable->GetName(), 
            EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);

        if (NewDataTable)
        {
            // 패키지에 포함된 모든 객체들을 언로드
            UPackage* Package = NewDataTable->GetOutermost();
            if (Package)
            {
                // 패키지를 메모리에서 언로드 (아직 레퍼런스가 있다면 실패할 수 있음)
                TArray<UPackage*> PackagesToUnload = { Package };
                PackageTools::UnloadPackages(PackagesToUnload);

                // 가비지 컬렉션 강제 호출
                CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
            }
        }

        //// 패키지 저장 후 참조 해제 및 가비지 컬렉션 실행
        NewDataTable->ClearFlags(RF_Public | RF_Standalone);
        NewDataTable = nullptr; // 참조 해제

        if (RowStruct)
        {
            // 패키지에 포함된 모든 객체들을 언로드
            UPackage* Package = RowStruct->GetOutermost();
            if (Package)
            {
                // 패키지를 메모리에서 언로드 (아직 레퍼런스가 있다면 실패할 수 있음)
                TArray<UPackage*> PackagesToUnload = { Package };
                PackageTools::UnloadPackages(PackagesToUnload);

                // 가비지 컬렉션 강제 호출
                CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
            }
        }
        
        //// 패키지 저장 후 참조 해제 및 가비지 컬렉션 실행
        RowStruct = nullptr; // 참조 해제
        

        //// 패키지 언로드
        if (SavedPackage)
        {
            TArray<UPackage*> PackagesToUnload = { SavedPackage };
            PackageTools::UnloadPackages(PackagesToUnload);
        }
        CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS); // 가비지 컬렉션 강제 실행

        doc.close();
        return true;
    }
    catch (const std::exception& ex)
    {
        UE_LOG(LogTemp, Error, TEXT("An error occurred while processing the Excel file: %s"), *FString(ex.what()));
        return false;
    }
}

// 해더파일 경로로 부터 데이터 구조체 정의(타입 정보)를 로드하고 클래스명을 반환하는 함수
UScriptStruct* SExcelImporterWidget::LoadStructFromHeaderPath(const FString& HeaderFilePath)
{
    // 헤더 파일 경로로부터 클래스명 추출
    FString ClassName = FPaths::GetBaseFilename(HeaderFilePath); // 예: MyRowStruct.h -> MyRowStruct

    // 모듈명 추출
    FString ModuleName = EditorPackageUtils::ExtractModuleNameFromPath(HeaderFilePath);

    if (ModuleName.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to extract module name from path: %s"), *HeaderFilePath);
        return nullptr;
    }

    // 동적으로 모듈명과 구조체명을 기반으로 구조체 정의(타입 정보)를 로드
    UScriptStruct* RowStruct = EditorPackageUtils::LoadStructDefinitionByName(ModuleName, ClassName);
    if (!RowStruct)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load RowStruct: %s / %s"), *ModuleName, *ClassName);
        return nullptr;
    }

    return RowStruct;
}
/**
 * 새로운 DataTable을 생성하거나 기존 DataTable이 존재하면 해당 DataTable의 모든 Row를 초기화하는 함수.
 * 만약 지정된 경로에 DataTable이 존재하지 않으면 새롭게 생성하여 반환합니다.
 *
 * @param RowStruct DataTable에 적용할 구조체(행 구조).
 * @param BaseClassName DataTable의 이름을 생성하기 위한 기본 클래스 이름.
 * @param DataTablePath 기존 DataTable을 찾기 위한 경로.
 * @return UDataTable* 새로 생성된 DataTable 또는 초기화된 기존 DataTable.
 */
UDataTable* SExcelImporterWidget::CreateNewOrClearDataTable(UScriptStruct* RowStruct, const FString& BaseClassName, const FString& DataTablePath)
{
    // DataTable 이름 생성 (예: DT_ClassName)
    FString DataTableName = FString::Printf(TEXT("DT_%s"), *BaseClassName);

    // 기존 DataTable을 경로에서 찾고, 모든 Row를 초기화
    UDataTable* NewDataTable = ClearDataTableRows(DataTablePath, DataTableName);

    if (!NewDataTable)
    {
        // 기존 DataTable이 없을 경우 새로운 DataTable을 생성
        NewDataTable = NewObject<UDataTable>(GetTransientPackage(), FName(*DataTableName), RF_Public | RF_Standalone);
        /**
        * 플래그 설명:
        *
        * 1. RF_Public
        * 설명: RF_Public는 객체가 외부에서 참조될 수 있음을 나타냅니다.
        * 사용 목적: 이 플래그가 설정된 객체는 다른 객체에서 참조될 수 있으며, 가비지 컬렉션 시스템이 이 객체를 수집할 때 참조 여부를 확인하여 수집되지 않도록 보호합니다.
        * 예시: 게임 내에서 여러 곳에서 참조되는 중요한 데이터나 오브젝트들이 RF_Public로 설정되어 있을 수 있습니다.
        *
        * 2. RF_Standalone
        * 설명: RF_Standalone 플래그는 객체가 독립적임을 의미하며, 이 플래그가 설정된 객체는 참조가 없더라도 가비지 컬렉션의 대상이 되지 않습니다.
        * 사용 목적: 이 플래그는 객체가 참조되지 않더라도 수집되지 않도록 보호합니다. 독립적으로 존재하고 메모리에서 유지될 필요가 있는 객체에 사용됩니다.
        * 예시: 게임의 중요한 설정 데이터, 외부 파일에서 로드된 에셋 등은 독립적으로 존재하고 참조 여부에 상관없이 유지될 필요가 있을 때 이 플래그를 사용합니다.
        */
        if (!NewDataTable)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create DataTable: %s"), *DataTableName);
            return nullptr;
        }

        // RowStruct를 DataTable에 설정 (행 구조 설정)
        NewDataTable->RowStruct = RowStruct;
    }

    return NewDataTable;
}


UDataTable* SExcelImporterWidget::ClearDataTableRows(const FString& DataTablePath, const FString& DataTableName)
{
    // DataTablePath 에서 PackagePath 로 경로 변환
    FString FilePath = FPaths::Combine(DataTablePath, DataTableName);
    FString PackagePath = EditorPackageUtils::ConvertFilePathToPackagePath(FilePath);
    UE_LOG(LogTemp, Log, TEXT("PackagePath: %s"), *PackagePath);

    // PackagePath 에서 UDataTable을 로드
    UDataTable* DataTable = LoadObject<UDataTable>(nullptr, *PackagePath);
    if (!DataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load DataTable from path: %s"), *PackagePath);
        return DataTable;
    }

    // DataTable에 로드된 모든 row를 가져옴
    TArray<FName> RowNames = DataTable->GetRowNames();
    if (RowNames.Num() == 0)
    {
        UE_LOG(LogTemp, Log, TEXT("DataTable is already empty or has no rows."));
        return DataTable;
    }

    // 모든 row를 초기화 또는 삭제 (여기서는 삭제 예시)
    for (const FName& RowName : RowNames)
    {
        DataTable->RemoveRow(RowName);
    }

    // 변경 사항을 반영하기 위해 DataTable을 수정된 상태로 표시
    DataTable->MarkPackageDirty();

    UE_LOG(LogTemp, Log, TEXT("Successfully cleared all rows from DataTable: %s"), *DataTablePath);
    return DataTable;
}

// Excel 데이터를 기반으로 FTableRowBase 생성 함수
FTableRowBase* SExcelImporterWidget::CreateDataTableRowFromExcel(
    /**
    * 절대로 직접 사용하던가 변경하단거 불가함
    * 절대 RowStruct 파라미터의 const 빼지 말것
    * FTableRowBase* NewRow = CreateDataTableRowFromExcel; 여기서 사용된거
    * 여기로 들어오는 RowStruct 파라미터는 새로 인스턴스 만들기 위한 참조용
    */
    const UScriptStruct* const RowStruct, 
    const OpenXLSX::XLWorksheet& wks, 
    int32 RowIndex)
{
    
    // 1. RowStruct의 크기만큼 메모리 할당 및 초기화
    FTableRowBase* NewRow = (FTableRowBase*)FMemory::Malloc(RowStruct->GetStructureSize());
    RowStruct->InitializeStruct(NewRow);

    // 2. 엑셀 열 인덱스를 관리하기 위한 변수
    int32 ColIndex = 2;  // 엑셀의 두 번째 열부터 시작 (첫 번째는 ID임)

    // 3. 구조체의 모든 필드에 대한 매핑 시작
    for (TFieldIterator<FProperty> It(RowStruct); It; ++It)
    {
        FProperty* StructProperty = *It;

        // 엑셀 데이터 읽기
        FString CellValue = FString(wks.cell(RowIndex, ColIndex).value().getString().c_str());

        // 엑셀 데이터가 비어 있으면 건너뜀
        if (CellValue.IsEmpty())
        {
            ColIndex++;
            continue;
        }

        // 4. 각 필드의 자료형에 따라 값을 설정
        if (FStrProperty* StringProperty = CastField<FStrProperty>(StructProperty))
        {
            StringProperty->SetPropertyValue_InContainer(NewRow, CellValue);
        }
        else if (FIntProperty* IntProperty = CastField<FIntProperty>(StructProperty))
        {
            IntProperty->SetPropertyValue_InContainer(NewRow, FCString::Atoi(*CellValue));
        }
        else if (FFloatProperty* FloatProperty = CastField<FFloatProperty>(StructProperty))
        {
            FloatProperty->SetPropertyValue_InContainer(NewRow, FCString::Atof(*CellValue));
        }
        else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(StructProperty))
        {
            BoolProperty->SetPropertyValue_InContainer(NewRow, CellValue.ToBool());
        }
        else if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(StructProperty))
        {
            DoubleProperty->SetPropertyValue_InContainer(NewRow, FCString::Atod(*CellValue));
        }
        else if (FByteProperty* ByteProperty = CastField<FByteProperty>(StructProperty))
        {
            ByteProperty->SetPropertyValue_InContainer(NewRow, FCString::Atoi(*CellValue));
        }
        else if (FObjectPropertyBase* ObjectPropertyBase = CastField<FObjectPropertyBase>(StructProperty))
        {
            // UTexture2D나 다른 UObject 타입 처리
            UObject* LoadedObject = LoadObject<UObject>(nullptr, *CellValue);
            if (LoadedObject && ObjectPropertyBase->PropertyClass->IsChildOf(LoadedObject->GetClass()))
            {
                ObjectPropertyBase->SetObjectPropertyValue_InContainer(NewRow, LoadedObject);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Unsupported object type or failed to load object: %s"), *CellValue);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Unsupported property type for field: %s"), *StructProperty->GetNameCPP());
        }

        // 다음 열로 이동
        ColIndex++;
    }

    // 5. 생성된 Row 반환
    return NewRow;
}

// DataTable을 어셋으로 저장하는 함수
void SExcelImporterWidget::SaveDataTableAsset(UDataTable* DataTable, const FString& DataTablePath)
{
    if (!DataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("DataTable is null!"));
        return;
    }

    // -- 프로젝트의 콘텐츠 디렉터리 경로를 절대 경로로 변환
    // DataTablePath 에서 PackagePath 로 경로 변환
    FString RelativePath = EditorPackageUtils::ConvertFilePathToPackagePath(DataTablePath);
    UE_LOG(LogTemp, Log, TEXT("Convert RelativePath: %s"), *RelativePath);
    if (RelativePath.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("DataTablePath is not inside any recognized content directory: %s (%s)"), *DataTablePath, *RelativePath);
        return;
    }

    // -- 패키지 경로에 DataTable 이름을 추가하여 최종 패키지 경로를 생성
    FString FullPackagePath = FPaths::Combine(RelativePath, DataTable->GetName());
    UE_LOG(LogTemp, Log, TEXT("Full Package Path: %s"), *FullPackagePath);

    //// -- 패키지 경로에 따라 처리
    //FString FilePath = SlateEditorUtils::PluginLongPackageNameToFilename(FullPackagePath);
    //UE_LOG(LogTemp, Log, TEXT("Saving to FilePath: %s"), *FilePath);

    //// -- 폴더가 존재하지 않으면 폴더 생성
    //FString DirectoryPath = FPaths::GetPath(FilePath);
    //if (!IFileManager::Get().DirectoryExists(*DirectoryPath))
    //{
    //    IFileManager::Get().MakeDirectory(*DirectoryPath, true);
    //    UE_LOG(LogTemp, Log, TEXT("Created directory: %s"), *DirectoryPath);
    //}

    if (!IFileManager::Get().DirectoryExists(*DataTablePath))
    {
        IFileManager::Get().MakeDirectory(*DataTablePath, true);
        UE_LOG(LogTemp, Log, TEXT("Created directory: %s"), *DataTablePath);
    }

    FString FilePath = FPaths::Combine(DataTablePath, DataTable->GetName());
    FilePath = EditorPackageUtils::EnsureUAssetExtension(FilePath);

    // -- 패키지 생성
    UPackage* ExistingPackage = FindPackage(nullptr, *FullPackagePath);
    if (!ExistingPackage)
    {
        // -- 패키지 생성
        ExistingPackage = CreatePackage(*FullPackagePath);
        if (!ExistingPackage)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create package: %s"), *FullPackagePath);
            return;
        }
        UE_LOG(LogTemp, Log, TEXT("Package created: %s"), *FullPackagePath);

        // -- 패키지에 DataTable을 포함시킴
        DataTable->Rename(*DataTable->GetName(), ExistingPackage);
    }

    DataTable->Rename(*DataTable->GetName(), ExistingPackage);

    // -- Asset 등록
    if (EditorPackageUtils::IsAssetAlreadyRegistered(FullPackagePath) == false)
    {
        FAssetRegistryModule::AssetCreated(DataTable);
    }

    // -- 패키지 저장
    DataTable->MarkPackageDirty();

    // -- 패키지 저장 처리
    bool bSaved = UPackage::SavePackage(ExistingPackage, DataTable, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *FilePath);

    if (!bSaved)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save package: %s"), *FilePath);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully saved package: %s"), *FilePath);
    }
}
// DataTable을 어셋으로 저장하는 함수
//void SExcelImporterWidget::SaveDataTableAsset(UDataTable* DataTable, const FString& DataTablePath)
//{
//    if (!DataTable)
//    {
//        UE_LOG(LogTemp, Error, TEXT("DataTable is null!"));
//        return;
//    }
//
//    // -- 프로젝트의 콘텐츠 디렉터리 경로를 절대 경로로 변환
//    FString ProjectContentDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
//    UE_LOG(LogTemp, Log, TEXT("Absolute Project Content Dir: %s"), *ProjectContentDir);
//
//    FString RelativePath = SlateEditorUtils::ConvertFilePathToPackagePath(DataTablePath);
//    if (RelativePath.IsEmpty())
//    {
//        UE_LOG(LogTemp, Error, TEXT("DataTablePath is not inside any recognized content directory: %s (%s)"), *DataTablePath, *RelativePath);
//        return;
//    }
//    else
//    {
//        UE_LOG(LogTemp, Log, TEXT("Convert RelativePath: %s"), *RelativePath);
//    }
//    // -- DataTablePath에서 콘텐츠 폴더 이후의 경로를 추출하여 /Game 경로로 변환
//    //FString RelativePath;
//    //bool bFound = false;
//
//    //// -- 프로젝트 콘텐츠 디렉터리 안에 있는지 확인
//    //if (DataTablePath.StartsWith(ProjectContentDir))
//    //{
//    //    // 프로젝트 콘텐츠 폴더 내에 있을 경우
//    //    RelativePath = DataTablePath.RightChop(ProjectContentDir.Len());
//
//    //    // -- 슬래시 중복 방지
//    //    if (RelativePath.StartsWith(TEXT("/")))
//    //    {
//    //        RelativePath.RemoveAt(0);
//    //    }
//
//    //    RelativePath = FString::Printf(TEXT("/Game/%s"), *RelativePath);
//    //    bFound = true;
//    //}
//    //else
//    //{
//    //    // -- 플러그인의 콘텐츠 디렉터리에서 확인
//    //    const TArray<TSharedRef<IPlugin>> Plugins = IPluginManager::Get().GetEnabledPlugins();
//    //    for (const TSharedRef<IPlugin>& Plugin : Plugins)
//    //    {
//    //        FString PluginContentDir = FPaths::ConvertRelativePathToFull(Plugin->GetContentDir());
//    //        if (DataTablePath.StartsWith(PluginContentDir))
//    //        {
//    //            // 플러그인의 콘텐츠 폴더 내에 있을 경우
//    //            RelativePath = DataTablePath.RightChop(PluginContentDir.Len());
//
//    //            // -- 슬래시 중복 방지
//    //            if (RelativePath.StartsWith(TEXT("/")))
//    //            {
//    //                RelativePath.RemoveAt(0); // 슬래시 제거
//    //            }
//
//    //            // 플러그인의 경로는 /Game/Plugins/로 변환
//    //            RelativePath = FString::Printf(TEXT("/%s/%s"), *Plugin->GetName(), *RelativePath);
//    //            bFound = true;
//    //            break;
//    //        }
//    //    }
//    //}
//
//    //UE_LOG(LogTemp, Log, TEXT("Final RelativePath: %s"), *RelativePath);
//
//    /*if (!bFound)
//    {
//        UE_LOG(LogTemp, Error, TEXT("DataTablePath is not inside any recognized content directory: %s"), *DataTablePath);
//        return;
//    }*/
//
//    // -- 패키지 경로에 DataTable 이름을 추가하여 최종 패키지 경로를 생성
//    FString FullPackagePath = FPaths::Combine(RelativePath, DataTable->GetName());
//    UE_LOG(LogTemp, Log, TEXT("Full Package Path: %s"), *FullPackagePath);
//
//    // -- 패키지 경로에 따라 처리
//    FString FilePath;
//    if (FullPackagePath.StartsWith(TEXT("/Game/Plugins/")))
//    {
//        // 플러그인 콘텐츠 폴더에 있는 경우
//        FilePath = SlateEditorUtils::PluginLongPackageNameToFilename(FullPackagePath);
//    }
//    else
//    {
//        // 프로젝트 콘텐츠 폴더에 있는 경우
//        FilePath = FPackageName::LongPackageNameToFilename(FullPackagePath, FPackageName::GetAssetPackageExtension());
//    }
//
//    UE_LOG(LogTemp, Log, TEXT("Saving to FilePath: %s"), *FilePath);
//
//    // -- 폴더가 존재하지 않으면 폴더 생성
//    FString DirectoryPath = FPaths::GetPath(FilePath);
//    if (!IFileManager::Get().DirectoryExists(*DirectoryPath))
//    {
//        IFileManager::Get().MakeDirectory(*DirectoryPath, true);
//        UE_LOG(LogTemp, Log, TEXT("Created directory: %s"), *DirectoryPath);
//    }
//
//    // -- 패키지 생성
//    UPackage* ExistingPackage = FindPackage(nullptr, *FullPackagePath);
//    if (ExistingPackage)
//    {
//        // 패키지가 이미 존재하는 경우, 기존 패키지를 사용함
//        /*UE_LOG(LogTemp, Warning, TEXT("Package already exists, using existing package: %s"), *FullPackagePath);
//        ExistingPackage->FullyLoad();*/
//    }
//    else
//    {
//        // 패키지가 존재하지 않으면 새로 생성
//        ExistingPackage = CreatePackage(*FullPackagePath);
//        if (!ExistingPackage)
//        {
//            UE_LOG(LogTemp, Error, TEXT("Failed to create package: %s"), *FullPackagePath);
//            return;
//        }
//        UE_LOG(LogTemp, Log, TEXT("Package created: %s"), *FullPackagePath);
//    }
//
//    // -- 패키지에 DataTable을 포함시킴
//    DataTable->Rename(*DataTable->GetName(), ExistingPackage);
//
//    // -- Asset 등록
//    FAssetRegistryModule::AssetCreated(DataTable);
//
//    // -- 패키지 저장
//    DataTable->MarkPackageDirty();
//
//    // -- 패키지 저장 처리
//    bool bSaved = UPackage::SavePackage(ExistingPackage, DataTable, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *FilePath);
//    if (!bSaved)
//    {
//        UE_LOG(LogTemp, Error, TEXT("Failed to save package: %s"), *FilePath);
//    }
//    else
//    {
//        UE_LOG(LogTemp, Log, TEXT("Successfully saved package: %s"), *FilePath);
//    }
//
//    // DataTable 참조 해제
//    DataTable = nullptr;
//
//    // 가비지 컬렉션 실행
//    CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
//
//    TArray<UPackage*> PackagesToUnload;
//    PackagesToUnload.Add(ExistingPackage);
//
//    // 패키지 언로드
//    PackageTools::UnloadPackages(PackagesToUnload);
//}
/*
void SExcelImporterWidget::SaveDataTableAsset(UDataTable* DataTable, const FString& DataTablePath)
{
    if (!DataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("DataTable is null!"));
        return;
    }

    // -- 프로젝트의 콘텐츠 디렉터리 경로를 절대 경로로 변환
    FString ProjectContentDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
    UE_LOG(LogTemp, Log, TEXT("Absolute Project Content Dir: %s"), *ProjectContentDir);

    // -- DataTablePath에서 콘텐츠 폴더 이후의 경로를 추출하여 /Game 경로로 변환
    FString RelativePath;
    bool bFound = false;

    // -- 프로젝트 콘텐츠 디렉터리 안에 있는지 확인
    if (DataTablePath.StartsWith(ProjectContentDir))
    {
        // 프로젝트 콘텐츠 폴더 내에 있을 경우
        RelativePath = DataTablePath.RightChop(ProjectContentDir.Len());

        // -- 슬래시 중복 방지
        if (RelativePath.StartsWith(TEXT("/")))
        {
            RelativePath.RemoveAt(0);
        }

        RelativePath = FString::Printf(TEXT("/Game/%s"), *RelativePath);
        bFound = true;
    }
    else
    {
        // -- 플러그인의 콘텐츠 디렉터리에서 확인
        const TArray<TSharedRef<IPlugin>> Plugins = IPluginManager::Get().GetEnabledPlugins();
        for (const TSharedRef<IPlugin>& Plugin : Plugins)
        {
            FString PluginContentDir = FPaths::ConvertRelativePathToFull(Plugin->GetContentDir());
            if (DataTablePath.StartsWith(PluginContentDir))
            {
                // 플러그인의 콘텐츠 폴더 내에 있을 경우
                RelativePath = DataTablePath.RightChop(PluginContentDir.Len());

                // -- 슬래시 중복 방지
                if (RelativePath.StartsWith(TEXT("/")))
                {
                    RelativePath.RemoveAt(0); // 슬래시 제거
                }

                // 플러그인의 경로는 /Game/Plugins/가 아니라 /Plugins/PluginName/으로 관리됨
                RelativePath = FString::Printf(TEXT("/Plugins/%s/%s"), *Plugin->GetName(), *RelativePath);
                bFound = true;
                break;
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Final RelativePath: %s"), *RelativePath);

    if (!bFound)
    {
        UE_LOG(LogTemp, Error, TEXT("DataTablePath is not inside any recognized content directory: %s"), *DataTablePath);
        return;
    }

    // -- 패키지 경로에 DataTable 이름을 추가하여 최종 패키지 경로를 생성
    FString FullPackagePath = FPaths::Combine(RelativePath, DataTable->GetName());
    UE_LOG(LogTemp, Log, TEXT("Full Package Path: %s"), *FullPackagePath);

    // -- 폴더가 존재하지 않으면 폴더 생성
    // 패키지 경로 생성 후 플러그인 경로일 경우 변환
    if (FullPackagePath.StartsWith(TEXT("/Plugins/")))
    {
        FullPackagePath = FullPackagePath.Replace(TEXT("/Plugins/"), TEXT("/Game/Plugins/"));
    }
    FString DirectoryPath = FPaths::GetPath(FPackageName::LongPackageNameToFilename(FullPackagePath, FPackageName::GetAssetPackageExtension()));    
    if (!IFileManager::Get().DirectoryExists(*DirectoryPath))
    {
        IFileManager::Get().MakeDirectory(*DirectoryPath, true);
        UE_LOG(LogTemp, Log, TEXT("Created directory: %s"), *DirectoryPath);
    }

    // -- 패키지 생성
    UPackage* ExistingPackage = FindPackage(nullptr, *FullPackagePath);
    if (ExistingPackage)
    {
        // 패키지가 이미 존재하는 경우, 기존 패키지를 사용함
        UE_LOG(LogTemp, Warning, TEXT("Package already exists, using existing package: %s"), *FullPackagePath);
        ExistingPackage->FullyLoad();
    }
    else
    {
        // 패키지가 존재하지 않으면 새로 생성
        ExistingPackage = CreatePackage(*FullPackagePath);
        if (!ExistingPackage)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create package: %s"), *FullPackagePath);
            return;
        }
        UE_LOG(LogTemp, Log, TEXT("Package created: %s"), *FullPackagePath);
    }

    // -- 패키지에 DataTable을 포함시킴
    DataTable->Rename(*DataTable->GetName(), ExistingPackage);

    // -- Asset 등록
    FAssetRegistryModule::AssetCreated(DataTable);

    // -- 패키지 저장
    DataTable->MarkPackageDirty();

    // -- 패키지 경로를 실제 파일 시스템 경로로 변환하여 저장
    FString FilePath = FPackageName::LongPackageNameToFilename(FullPackagePath, FPackageName::GetAssetPackageExtension());
    UE_LOG(LogTemp, Log, TEXT("Saving to FilePath: %s"), *FilePath);

    bool bSaved = UPackage::SavePackage(ExistingPackage, DataTable, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *FilePath);

    if (!bSaved)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save package: %s"), *FilePath);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Successfully saved package: %s"), *FilePath);
    }
}
*/