# ExcelImporter

Please download the following repositories at the same level as `Unreal_ExcelImporter`:

- [Unreal_SlateEditorUtils](https://github.com/dipi0123/Unreal_SlateEditorUtils.git)
- [Unreal_EditorPackageUtils](https://github.com/dipi0123/Unreal_EditorPackageUtils.git)

Make sure that all three repositories are in the same folder structure, like this:

```
C:/Unreal Projects/excel_plugin/Plugins/
    ├── Unreal_ExcelImporter/
    ├── Unreal_SlateEditorUtils/
    └── Unreal_EditorPackageUtils/
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

## 사용 방법
