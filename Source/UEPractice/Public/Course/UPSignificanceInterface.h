// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UPSignificanceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
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

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Significance")
	bool GetSignificanceLocation(FTransform Viewpoint, FVector& OutLocation);

	UFUNCTION(BlueprintNativeEvent, Category = "Significance")
	bool CalcSignificanceOverride(float& OutSignificance);
};
