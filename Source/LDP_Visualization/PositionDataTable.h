// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "PositionDataTable.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct FPositionDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FPositionDataTable(): XPos(0), YPos(0), ZPos(0)
	{
	}

	/** XP to get to the given level from the previous level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=LevelUp)
	float XPos;

	/** XP to get to the given level from the previous level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=LevelUp)
	float YPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=LevelUp)
	float ZPos;

	FVector GetPos() const
	{
		return FVector(XPos, YPos, ZPos);
	}
};
