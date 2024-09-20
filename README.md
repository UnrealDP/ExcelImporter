# ExcelImporter

Please download the following repositories at the same level as `Unreal ExcelImporter`:

- [Unreal SlateEditorUtils](https://github.com/UnrealDP/SlateEditorUtils.git)
- [Unreal EditorPackageUtils](https://github.com/UnrealDP/EditorPackageUtils.git)

Make sure that all three repositories are in the same folder structure, like this:

```
C:/Unreal Projects/excel_plugin/Plugins/
    ├── ExcelImporter/
    ├── SlateEditorUtils/
    └── EditorPackageUtils/
```

<details>
<summary>English Instructions</summary>

## Instructions

1. Define data in `Plugins/ExcelImporter/Content/DT_DataTypeSettings.uasset`.  
   Here’s how to structure the data:

   - **RowName**: The string key value representing the data type in Excel.
   - **UnrealCodeDataType**: The actual data type used in Unreal Engine (based on C++ types).  
   
   Example:

   | RowName      | UnrealCodeDataType |
   |--------------|--------------------|
   | `Bool`       | `bool`             |
   | `Int`        | `int32`            |
   | `Float`      | `float`            |
   | `Texture2D`  | `UTexture2D*`      |
   | `SkeletalMesh`| `USkeletalMesh*`  |

2. Define the import settings in `Plugins/ExcelImporter/Content/DT_ExcelImportSettings.uasset`.  
   The following fields must be set:

   - **RowName**: The filename of the final DataTable to be created.
   - **ExcelFilePath**: The path to the original Excel file to process (including the `.xlsx` extension).
   - **SheetName**: The name of the sheet to extract from the Excel file.
   - **GeneratedCodePath**: The path where the generated header file (which inherits from `FTableRowBase`) will be saved (including the `.h` extension).
   - **DataTablePath**: The folder path where the final DataTable will be saved (do not include the filename).

3. Open the Unreal Editor and go to `Window > ImportExcelData` to open the import popup.

4. Select the checkbox for the Excel file you wish to convert.

5. Click the `Generate Selected to C++ Header` button.

6. Once the header file is generated, the Unreal Editor will automatically restart to ensure correct application.

7. After restarting, open `Window > ImportExcelData` in the Unreal Editor again.

8. Select the checkbox for the Excel data you want to convert to a DataTable.

9. Click the `Create DataTable` button to generate the DataTable.

## Known Issues

- When you first create the DataTable, it may be referenced with a count of 1, causing an issue where you are unable to delete the DataTable without using the Force Delete option.
- If you encounter this issue and need to delete the DataTable, you can either restart the editor before attempting to delete the DataTable or use Force Delete and restart the editor afterward.

</details>

<details>
<summary>한글 설명</summary>
<summary>한글 설명</summary>

## 사용 방법

1. `Plugins/ExcelImporter/Content/DT_DataTypeSettings.uasset`에 데이터를 정의합니다.  
   데이터 구조는 다음과 같습니다:

   - **RowName**: Excel에서 사용될 자료형의 문자열 키 값.
   - **UnrealCodeDataType**: Unreal Engine에서 실제로 사용되는 자료형(C++ 코드 기준).  
   
   예시:

   | RowName      | UnrealCodeDataType |
   |--------------|--------------------|
   | `Bool`       | `bool`             |
   | `Int`        | `int32`            |
   | `Float`      | `float`            |
   | `Texture2D`  | `UTexture2D*`      |
   | `SkeletalMesh`| `USkeletalMesh*`  |

2. `Plugins/ExcelImporter/Content/DT_ExcelImportSettings.uasset`에 파일과 폴더 경로를 설정합니다.  
   설정할 필드는 다음과 같습니다:

   - **RowName**: 최종적으로 생성될 DataTable 파일명.
   - **ExcelFilePath**: 원본 Excel 파일 경로(.xlsx 확장자 포함).
   - **SheetName**: Excel 파일에서 추출할 시트 이름.
   - **GeneratedCodePath**: 생성된 헤더 파일이 저장될 경로(.h 확장자 포함).
   - **DataTablePath**: 최종적으로 생성될 DataTable의 폴더 경로(파일명은 미포함).

3. Unreal Editor에서 `Window > ImportExcelData`로 팝업을 엽니다.

4. 변환할 Excel 파일의 체크박스를 선택합니다.

5. `Generate Selected to C++ Header` 버튼을 클릭합니다.

6. 헤더 파일이 생성되면, Unreal Editor가 자동으로 재시작되어 올바르게 적용됩니다.

7. 재시작 후 다시 `Window > ImportExcelData`에서 팝업을 엽니다.

8. Excel 데이터를 DataTable로 변환할 파일의 체크박스를 선택합니다.

9. `Create DataTable` 버튼을 클릭하여 DataTable을 생성합니다.

## 알려진 문제

- DataTable을 처음 생성할 때 참조 횟수가 1로 설정되어, 바로 삭제할 수 없는 문제가 발생할 수 있습니다. 이 경우, Force Delete를 사용하거나 Unreal Editor를 재시작한 후 삭제할 수 있습니다.

</details>
