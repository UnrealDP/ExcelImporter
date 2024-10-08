#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Sample1.generated.h"

USTRUCT(BlueprintType)
struct FSample1  : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Name = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 Num =  0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Score = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UTexture2D* IconTexture = nullptr;

};
