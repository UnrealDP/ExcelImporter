// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * This is the module definition for the editor mode. You can implement custom functionality
 * as your plugin module starts up and shuts down. See IModuleInterface for more extensibility options.
 */
class FExcelImporterModule : public IModuleInterface
{
private:
	void RegisterMenus();
	void OpenExcelImporter();

public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void ConvertExcelToCSV(const FString& ExcelFilePath, const FString& CSVFilePath, const FString& SheetName);
};
