// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExcelImporterEditorModeToolkit.h"
#include "ExcelImporterEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "ExcelImporterEditorModeToolkit"

FExcelImporterEditorModeToolkit::FExcelImporterEditorModeToolkit()
{
}

void FExcelImporterEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FExcelImporterEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FExcelImporterEditorModeToolkit::GetToolkitFName() const
{
	return FName("ExcelImporterEditorMode");
}

FText FExcelImporterEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "ExcelImporterEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
