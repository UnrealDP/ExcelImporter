#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Sample2.generated.h"

USTRUCT(BlueprintType)
struct FSample2  : public FTableRowBase
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
