// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "UPSignificanceSettings.generated.h"


USTRUCT()
struct FSignificanceBucketInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<int32> BucketSizes;
};

UCLASS(Config=Game, defaultconfig, meta = (DisplayName="UP Significance Manager")) // 'defaultconfig' = "Save object config only to Default INIs, never to local INIs."
class UEPRACTICE_API UUPSignificanceSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()
	
public:
	/*
	 * Return Current Bucket index based on the sorted significance index we pass in and the maximum bucket sizes specified in the Settings. 
	 */
	int32 GetBucketIndex(FName InTag, int32 InIndex) const;

	//~UDeveloperSettings interface
	//virtual void PostInitProperties() override;

	virtual FName GetCategoryName() const override;
	//~End of UDeveloperSettings interface

protected:
	
	UPROPERTY(EditAnywhere, Config)
	TMap<FName, FSignificanceBucketInfo> Buckets;
};
