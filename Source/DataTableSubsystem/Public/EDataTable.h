// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EDataTable.generated.h"

UENUM(BlueprintType)
enum class EDataTable : uint8
{
	SkillData UMETA(DisplayName = "SkillData"),
	Max   UMETA(DisplayName = "Max Data")
};
