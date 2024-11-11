#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TestSample.generated.h"

USTRUCT(BlueprintType)
struct FTestStruct2
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float TestFloat;

};

USTRUCT(BlueprintType)
struct FTestStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float TestFloat;

};

USTRUCT(BlueprintType)
struct FTestSample : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 Num = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TArray<FTestStruct> TestSamples;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FTestStruct2 Struct;	
};
