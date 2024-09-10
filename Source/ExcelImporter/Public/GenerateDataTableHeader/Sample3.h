#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Sample3.generated.h"

USTRUCT(BlueprintType)
struct FSample3  : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 Num;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UTexture2D* IconTexture;

};
