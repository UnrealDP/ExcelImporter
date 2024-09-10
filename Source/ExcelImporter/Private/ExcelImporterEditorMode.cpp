// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExcelImporterEditorMode.h"
#include "ExcelImporterEditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "ExcelImporterEditorModeCommands.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
#include "Tools/ExcelImporterSimpleTool.h"
#include "Tools/ExcelImporterInteractiveTool.h"

// step 2: register a ToolBuilder in FExcelImporterEditorMode::Enter() below


#define LOCTEXT_NAMESPACE "ExcelImporterEditorMode"

const FEditorModeID UExcelImporterEditorMode::EM_ExcelImporterEditorModeId = TEXT("EM_ExcelImporterEditorMode");

FString UExcelImporterEditorMode::SimpleToolName = TEXT("ExcelImporter_ActorInfoTool");
FString UExcelImporterEditorMode::InteractiveToolName = TEXT("ExcelImporter_MeasureDistanceTool");


UExcelImporterEditorMode::UExcelImporterEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(UExcelImporterEditorMode::EM_ExcelImporterEditorModeId,
		LOCTEXT("ModeName", "ExcelImporter"),
		FSlateIcon(),
		true);
}


UExcelImporterEditorMode::~UExcelImporterEditorMode()
{
}


void UExcelImporterEditorMode::ActorSelectionChangeNotify()
{
}

void UExcelImporterEditorMode::Enter()
{
	UEdMode::Enter();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FExcelImporterEditorModeCommands& SampleToolCommands = FExcelImporterEditorModeCommands::Get();

	RegisterTool(SampleToolCommands.SimpleTool, SimpleToolName, NewObject<UExcelImporterSimpleToolBuilder>(this));
	RegisterTool(SampleToolCommands.InteractiveTool, InteractiveToolName, NewObject<UExcelImporterInteractiveToolBuilder>(this));

	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, SimpleToolName);
}

void UExcelImporterEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FExcelImporterEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UExcelImporterEditorMode::GetModeCommands() const
{
	return FExcelImporterEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
