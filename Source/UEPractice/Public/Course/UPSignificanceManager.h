// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SignificanceManager.h"
#include "UPSignificanceManager.generated.h"


struct FExtendedManagedObjectInfo : USignificanceManager::FManagedObjectInfo
{
	FExtendedManagedObjectInfo(
		UObject* InObject, 
		FName InTag, 
		USignificanceManager::FManagedObjectSignificanceFunction InSignificanceFunction, 
		USignificanceManager::EPostSignificanceType InPostSignificanceType = USignificanceManager::EPostSignificanceType::None, 
		USignificanceManager::FManagedObjectPostSignificanceFunction InPostSignificanceFunction = nullptr
	)
		: FManagedObjectInfo(InObject, InTag, InSignificanceFunction, InPostSignificanceType, InPostSignificanceFunction)
	{
		LOD = 0;
	}

public:
	int32 LOD;
};

UCLASS()
class UEPRACTICE_API UUPSignificanceManager : public USignificanceManager
{
	GENERATED_BODY()
	
public:
	virtual void Update(TArrayView<const FTransform> InViewpoints) override;

	virtual void RegisterObject(UObject* Object, FName Tag, FManagedObjectSignificanceFunction SignificanceFunction, EPostSignificanceType InPostSignificanceType = EPostSignificanceType::None,
		FManagedObjectPostSignificanceFunction InPostSignificanceFunction = nullptr) override;

	virtual void UnregisterObject(UObject* Object) override;

protected:
	TArray<FManagedObjectInfo*> ChangedLODs;

	mutable FCriticalSection RegisteredTagsMutex;
	TArray<FName> RegisteredTags;
	TMultiMap<UObject*, FName> RegistrationMap;  // Tracks which tags each object registered
};
