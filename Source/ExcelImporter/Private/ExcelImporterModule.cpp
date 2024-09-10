// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExcelImporterModule.h"
#include "ExcelImporterEditorModeCommands.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "ExcelImporterWidget.h"


#define LOCTEXT_NAMESPACE "ExcelImporterModule"

void FExcelImporterModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FExcelImporterEditorModeCommands::Register();

    RegisterMenus();
}

void FExcelImporterModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FExcelImporterEditorModeCommands::Unregister();
}

void FExcelImporterModule::ConvertExcelToCSV(const FString& ExcelFilePath, const FString& CSVFilePath, const FString& SheetName)
{
    FString ExecutablePath = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("ExcelImporter/ThirdParty/ExcelToCSVConverter/ExcelToCSVConverter.exe"));
    FString CommandLine = FString::Printf(TEXT("\"%s\" \"%s\" \"%s\""), *ExcelFilePath, *CSVFilePath, *SheetName);

    int32 ExitCode = -1; // Initialize ExitCode
    FProcHandle ProcessHandle = FPlatformProcess::CreateProc(*ExecutablePath, *CommandLine, true, false, false, nullptr, 0, nullptr, nullptr);

    if (ProcessHandle.IsValid())
    {
        FPlatformProcess::WaitForProc(ProcessHandle);
        FPlatformProcess::GetProcReturnCode(ProcessHandle, &ExitCode); // Retrieve the exit code
        FPlatformProcess::CloseProc(ProcessHandle);

        if (ExitCode == 0)
        {
            UE_LOG(LogTemp, Log, TEXT("Successfully converted Excel to CSV"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to convert Excel to CSV. Exit code: %d"), ExitCode);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to start process for converting Excel to CSV"));
    }
}


void FExcelImporterModule::RegisterMenus()
{
    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");

    FToolMenuSection& Section = Menu->AddSection("ExcelImporterSection", LOCTEXT("ExcelImporterSection", "Excel Importer"));
    Section.AddMenuEntry("ImportExcelData",
        LOCTEXT("ImportExcelDataMenuEntry", "Import Excel Data"),
        LOCTEXT("ImportExcelDataMenuEntryTooltip", "Opens the Excel Importer"),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FExcelImporterModule::OpenExcelImporter)));
}

void FExcelImporterModule::OpenExcelImporter()
{
    // Create and display the ExcelImporterWidget
    TSharedRef<SWindow> Window = SNew(SWindow)
        .Title(FText::FromString("Excel Importer"))
        .ClientSize(FVector2D(800, 600))
        [
            SNew(SExcelImporterWidget)
        ];

    // Show window
    FSlateApplication::Get().AddWindow(Window);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FExcelImporterModule, ExcelImporterEditorMode)