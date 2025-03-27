// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SignificanceManager.h"
#include "Components/ActorComponent.h"
#include "UPSignificanceComponent.generated.h"



UENUM(BlueprintType)
enum class ESignificanceValue : uint8
{
	Hidden = 0, // Special tier while owning Actor is hidden ingame
	Lowest = 1, // Assumed very far away
	Medium = 2, // no longer critical
	Highest = 3 // No stripping/culling
};

// Trigger only when significance has changed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPostSignificanceChanged, ESignificanceValue, Significance);

/* Configure mapping between Significance and Distance to. While distance from viewpoint is within "MaxDistance" the specified "Significance" will be used. */
USTRUCT(BlueprintType)
struct FSignificanceDistance
{
	GENERATED_BODY()

public:
	FSignificanceDistance()
	{
		Significance = ESignificanceValue::Highest;
		MaxDistance = 1000.f;
	}
	
	FSignificanceDistance(ESignificanceValue InSignificance, float InMaxDistance) :
		Significance(InSignificance),
		MaxDistance(InMaxDistance) {}

public:
	float GetMaxDistSqrd() const { return MaxDistance*MaxDistance;}

public:
	UPROPERTY(EditAnywhere)
	ESignificanceValue Significance;

	UPROPERTY(EditAnywhere, meta = (UIMin=1000.0, UIMax=50000))
	float MaxDistance;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), HideCategories=("Collision", "Tags", "ComponentTick", "ComponentReplication", "Cooking", "Activation", "AssetUserData") )
class UEPRACTICE_API UUPSignificanceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUPSignificanceComponent();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UFUNCTION()
	void RegisterWithManager();

	float CalcSignificance(USignificanceManager::FManagedObjectInfo* ObjectInfo, const FTransform& Viewpoint) const;
	
	void PostSignificanceUpdate(USignificanceManager::FManagedObjectInfo* ObjectInfo, float OldSignificance, float Significance, bool bFinal);

	float GetSignificanceByDistance(float DistanceSqrd) const;

	void UpdateParticleSignificance(float NewSignificance);

public:
	
	/* Register with Significance Manager to optimize ticks/logic (Allows per (child)class / instance opt-out) */
	UPROPERTY(EditAnywhere, Category = "Optimization")
	bool bManageSignificance;
	
	/* MaxDistance per Significance, defaults to 0.0 (lowest) Significance if outside bounds of distances specified here  */
	UPROPERTY(EditAnywhere, meta = (TitleProperty="MaxDistance"))
	TArray<FSignificanceDistance> Thresholds;

	UPROPERTY(BlueprintReadOnly, Category= "Optimization")
	ESignificanceValue CurrentSignificance;

protected:
	
	/* Considered insignificant (-1.0) while owning Actor is HiddenInGame */
	UPROPERTY(EditAnywhere, Category = "Optimization")
	bool bInsignificantWhenOwnerIsHidden;
	
	/* Delay registration by one tick to allow actors/components to tick once, this may be useful to have them setup VFX beams etc. */
	UPROPERTY(EditDefaultsOnly, Category = "Optimization")
	bool bWaitOneFrame;

	UPROPERTY(EditDefaultsOnly, Category = "Optimization")
	bool bManageOwnerParticleSignificance;

	bool bHasImplementedInterface;

public:
	UPROPERTY(BlueprintAssignable)
	FOnPostSignificanceChanged OnSignificanceChanged;
};
