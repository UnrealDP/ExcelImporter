// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EDataTable.generated.h"

UENUM(BlueprintType)
enum class EDataTable : uint8
{
	SkillData UMETA(DisplayName = "SkillData"),
	SkillShowData UMETA(DisplayName = "SkillShowData"),
	EffectData UMETA(DisplayName = "EffectData"),
	Max   UMETA(DisplayName = "Max Data")
};

template<typename T>
struct TypeIndex
{
    static EDataTable GetType()
    {
        static_assert(sizeof(T) == 0, "Type must be specialized for each type.");
        return EDataTable::Max;  // 기본값
    }

    static int32 GetIndex()
    {
        static_assert(sizeof(T) == 0, "TypeIndex must be specialized for each type.");
        return -1;  // 기본값
    }
};

// 매크로 정의: 각 클래스에 대해 템플릿 특수화 적용
#define DATATABLE_TYPE_INDEX(ClassType, EnumValue)      \
template<>                                           \
struct TypeIndex<ClassType>                          \
{                                                    \
    static EDataTable GetType() { return EnumValue; } \
    static int32 GetIndex() { return static_cast<int32>(EnumValue); } \
};