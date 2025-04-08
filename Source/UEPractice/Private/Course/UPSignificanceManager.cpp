// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPSignificanceManager.h"
#include "Course/UPSignificanceInterface.h"
#include "Course/UPSignificanceSettings.h"


void UUPSignificanceManager::Update(TArrayView<const FTransform> InViewpoints)
{
	// Base calculates and sorts the significance
	Super::Update(InViewpoints);

	// Fetch Buckets
	const UUPSignificanceSettings* Settings = GetDefault<UUPSignificanceSettings>();

	// Keep the objects that changed LODs this update
	ChangedLODs.Empty(ChangedLODs.Num());

	for (int TagIndex = 0; TagIndex < RegisteredTags.Num(); ++TagIndex)
	{
		const TArray<USignificanceManager::FManagedObjectInfo*>& SortedObjects = GetManagedObjects(RegisteredTags[TagIndex]);
		for (int Index = 0; Index < SortedObjects.Num(); ++Index)
		{
			int32 NewLOD = Settings->GetBucketIndex(RegisteredTags[TagIndex], Index);

			FExtendedManagedObject* ExtObj = static_cast<FExtendedManagedObject*>(SortedObjects[Index]);
			if (ExtObj->LOD != NewLOD)
			{
				ChangedLODs.Add(ExtObj);
				ExtObj->LOD = NewLOD;
			}
		}

		// We can now broadcast LOD changes to individual Actors
		for (FManagedObjectInfo* ObjectInfo : ChangedLODs)
		{
			FExtendedManagedObject* ExtObj = static_cast<FExtendedManagedObject*>(ObjectInfo);

			// We could register components for cache performance, in that case the interface should still be called on the Owning Actor
			UObject* ObjectInst = ObjectInfo->GetObject();
			if (ObjectInfo->GetObject()->IsA(UActorComponent::StaticClass()))
			{
				ObjectInst = CastChecked<UActorComponent>(ObjectInfo->GetObject())->GetOwner();
			}

			IUPSignificanceInterface* ObjInterface = Cast<IUPSignificanceInterface>(ObjectInst);
			ObjInterface->SignificanceLODChanged(ExtObj->LOD);
		}
	}
}

void UUPSignificanceManager::RegisterObject(UObject* Object, FName Tag,
	FManagedObjectSignificanceFunction SignificanceFunction, EPostSignificanceType InPostSignificanceType,
	FManagedObjectPostSignificanceFunction InPostSignificanceFunction)
{
	Super::RegisterObject(Object, Tag, SignificanceFunction, InPostSignificanceType, InPostSignificanceFunction);

	// Bookkeeping for iterating the sorted lists easily
	RegisteredTags.AddUnique(Tag);

	// @todo: verify that there is a bucket available for this "Tag"
	// const URogueSignificanceSettings* Settings = GetDefault<URogueSignificanceSettings>();
}

void UUPSignificanceManager::UnregisterObject(UObject* Object)
{
	Super::UnregisterObject(Object);

	// @todo: Cleanup the RegisteredTags array
}
