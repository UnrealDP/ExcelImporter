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

# ExcelImporter

Please download the following repositories at the same level as `Unreal_ExcelImporter`:

- [Unreal_SlateEditorUtils](https://github.com/dipi0123/Unreal_SlateEditorUtils.git)
- [Unreal_EditorPackageUtils](https://github.com/dipi0123/Unreal_EditorPackageUtils.git)

Ensure that all three repositories are in the same folder structure, as shown below:






1. Plugins\ExcelImporter\Content 폴더의 DT_DataTypeSettings.uasset 에 데이터를 정의합니다.
    RowName : 액셀에 표기할 자료형의 문자열 Key값
    UnrealCodeDataType : 얼리얼에서 사용되는 실제 자료형 (C++ 코드 기준)
    Ex) Bool            /   bool
        Int             /   int32
        Float           /   float
        Texture2D       /   UTexture2D*
        SkeletalMesh    /   USkeletalMesh*

2. Plugins\ExcelImporter\Content 폴더의 DT_ExcelImportSettings.uasset 에 Import 할 파일과 폴더 등을 지정합니다.
    RowName : 최종 생성하고자 하는 DataTable 파일명
    ExcelFilePath : 가공에 필요한 원본 액셀파일 경로 (.xlsx 확장자 포함)
    SheetName : 액셀파일에서 추출하고자 하는 Sheet명
    GeneratedCodePath : 액셀 기준으로 생성한 FTableRowBase 를 상속받은 해더파일이 저장될 경로 (.h 확장자 포함)
    DataTablePath : 최종 생성될 데이터 테이블의 경로 (저장될 폴더 경로 입력, 파일명은 미기입)

3. 언리얼 에디터 Window > ImportExcelData 로 팝업 오픈
4. 변환할 액셀파일 체크박스 선택
5. Generated Selected to C++ Header 버튼 클릭
6. 해더 파일 완성되면 빌드 후 정확한 적용을 위해 자동으로 언리얼 에디저 다시 실행됨
7. 언리얼 에디터 Window > ImportExcelData 로 팝업 오픈
8. 액셀에 있는 데이터를 DataTable 로 변환할 액셀을 체크박스 선택
9. Create DataTable 버튼을 눌러서 데이터 생성

- 해결해야될 이슈
    최초 DataTable 생성하였을 때 Referrence 가 1로 카운딩되어 있어 생성한 DataTable 을 바로 삭제하려고 하면 Force Delete가 뜬다.
    생겅하고 바로 삭제해야 하는 이슈가 발생하면 삭제 전에 에디터를 다시 실행하거나 혹은 삭제할 DataTable을 Force Delete 하고 에디터를 다시 실행하기를 권장한다.