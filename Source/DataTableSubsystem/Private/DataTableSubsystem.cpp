#include "DataTableSubsystem.h"

DEFINE_LOG_CATEGORY(DataTableSubsystem);

#define LOCTEXT_NAMESPACE "FDataTableSubsystem"

void FDataTableSubsystem::StartupModule()
{
	UE_LOG(DataTableSubsystem, Warning, TEXT("DataTableSubsystem module has been loaded"));
}

void FDataTableSubsystem::ShutdownModule()
{
	UE_LOG(DataTableSubsystem, Warning, TEXT("DataTableSubsystem module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDataTableSubsystem, DataTableSubsystem)