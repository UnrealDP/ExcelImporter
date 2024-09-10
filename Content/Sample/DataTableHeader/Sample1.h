#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

USTRUCT(BlueprintType)
struct FGeneratedTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UnknownType RowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 Num;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UTexture2D* IconTexture;

};
