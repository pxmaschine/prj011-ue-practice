// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UPSignificanceInterface.generated.h"

// Marked not blueprintable, this lets us use it more easily and hopefully more performant. All usage simply need a base class in C++
UINTERFACE(MinimalAPI/*, meta=(CannotImplementInterfaceInBlueprint)*/)
class UUPSignificanceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UEPRACTICE_API IUPSignificanceInterface
{
	GENERATED_BODY()

public:
	virtual void SignificanceLODChanged(int32 NewLOD) = 0;

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Significance")
	bool GetSignificanceLocation(FTransform Viewpoint, FVector& OutLocation);

	UFUNCTION(BlueprintNativeEvent, Category = "Significance")
	bool CalcSignificanceOverride(float& OutSignificance);
};
