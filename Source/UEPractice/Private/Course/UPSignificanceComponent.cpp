// Fill out your copyright notice in the Description page of Project Settings.


#include "UEPractice/UEPractice.h"

#include "Course/UPSignificanceComponent.h"
#include "Course/UPSignificanceInterface.h"

#include "ParticleHelper.h"
#include "Particles/ParticleSystemComponent.h"


/* Allows us to force significance on all classes to quickly compare the performance differences as if the system was disabled */
static float GForcedSignificance = -1;
static FAutoConsoleVariableRef CVarSignificanceManager_ForceSignificance(
	TEXT("SigMan.ForceSignificance"),
	GForcedSignificance,
	TEXT("Force significance on all managed objects. -1 is default, 0-4 is hidden, lowest, medium, highest.\n"),
	ECVF_Cheat
	);

UUPSignificanceComponent::UUPSignificanceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWaitOneFrame = true;
	bManageSignificance = true;
	bInsignificantWhenOwnerIsHidden = true;
	bManageOwnerParticleSignificance = true;

	// Adding 0.0/Lowest is optional, anything beyond last entry distance will fallback to Lowest significance
	Thresholds.Emplace(ESignificanceValue::Highest, 5000.f);
}

void UUPSignificanceComponent::BeginPlay()
{
	Super::BeginPlay();

	
	// Register
	if (bManageSignificance)
	{
		bHasImplementedInterface = GetOwner()->Implements<UUPSignificanceInterface>();

		// Optionally wait one frame
		if (bWaitOneFrame)
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UUPSignificanceComponent::RegisterWithManager);
		}
		else
		{
			RegisterWithManager();
		}

		// Manage Cascade (deprecated) particle components
		if (bManageOwnerParticleSignificance)
		{
			TArray<UParticleSystemComponent*> Particles;
			GetOwner()->GetComponents<UParticleSystemComponent>(Particles);

			for (UParticleSystemComponent* Comp : Particles)
			{
				Comp->SetManagingSignificance(true);
			}
		}
	}
}

void UUPSignificanceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bManageSignificance)
	{
		// Unregister
		if (USignificanceManager* SignificanceManager = USignificanceManager::Get(GetWorld()))
		{
			SignificanceManager->UnregisterObject(this);
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void UUPSignificanceComponent::RegisterWithManager()
{
	if (USignificanceManager* SignificanceManager = USignificanceManager::Get(GetWorld()))
	{
		auto SignificanceFunc = [&](USignificanceManager::FManagedObjectInfo* ObjectInfo, const FTransform& Viewpoint) -> float
		{
			return CalcSignificance(ObjectInfo, Viewpoint);
		};
		
		auto PostSignificanceFunc = [&](USignificanceManager::FManagedObjectInfo* ObjectInfo, float OldSignificance, float Significance, bool bFinal)
		{
			PostSignificanceUpdate(ObjectInfo, OldSignificance, Significance, bFinal);
		};

		// Register
		FName Tag = GetOwner()->GetClass()->GetFName();
		SignificanceManager->RegisterObject(this, Tag, SignificanceFunc, USignificanceManager::EPostSignificanceType::Sequential, PostSignificanceFunc);
	}
}

float UUPSignificanceComponent::CalcSignificance(USignificanceManager::FManagedObjectInfo* ObjectInfo,
	const FTransform& Viewpoint) const
{
	if (GForcedSignificance >= 0.0f)
	{
		return GForcedSignificance;
	}
	
	if (bInsignificantWhenOwnerIsHidden && GetOwner()->IsHidden())
	{
		return static_cast<float>(ESignificanceValue::Hidden);
	}

	// Use Actor implemented override if present
	if (bHasImplementedInterface)
	{
		float Significance = 0.0f;
		if (IUPSignificanceInterface::Execute_CalcSignificanceOverride(GetOwner(), Significance))
		{
			return Significance;
		}
	}

	FVector OriginLocation = GetOwner()->GetActorLocation();
	if (bHasImplementedInterface)
	{
		FVector OverriddenOrigin = FVector::ZeroVector;
		if (IUPSignificanceInterface::Execute_GetSignificanceLocation(GetOwner(), Viewpoint, OverriddenOrigin))
		{
			OriginLocation = OverriddenOrigin;
		}
	}
	
	const float DistanceSqrd = (OriginLocation - Viewpoint.GetLocation()).SizeSquared();
	return GetSignificanceByDistance(DistanceSqrd);
}

void UUPSignificanceComponent::PostSignificanceUpdate(USignificanceManager::FManagedObjectInfo* ObjectInfo,
	float OldSignificance, float Significance, bool bFinal)
{
	if (OldSignificance == Significance)
	{
		//return;
	}

	if (Significance != OldSignificance)
	{
		CurrentSignificance = static_cast<ESignificanceValue>(Significance);
		UE_LOG(LogTemp, Log, TEXT("Significance for %s changed to %s"), *GetNameSafe(GetOwner()), *UEnum::GetValueAsString(CurrentSignificance));
		OnSignificanceChanged.Broadcast(CurrentSignificance);
	}

	// Running this every frame for now as otherwise we can't properly cull the cascade VFX (to be re-worked later)
	UpdateParticleSignificance(Significance);
}

float UUPSignificanceComponent::GetSignificanceByDistance(float DistanceSqrd) const
{
	const int32 NumThresholds = Thresholds.Num();
	if (NumThresholds == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SignificanceManager: No distance thresholds set in %s."), *GetNameSafe(GetOwner()));
		return static_cast<float>(ESignificanceValue::Highest);
	}

	// @todo: implement dist multiplier for even better tweaking
	if (DistanceSqrd >= Thresholds[NumThresholds-1].GetMaxDistSqrd())
	{
		// Max distance reached assume lowest significance
		return static_cast<float>(ESignificanceValue::Lowest);
	}

	for (int32 Idx = 0; Idx < NumThresholds; Idx++)
	{
		const FSignificanceDistance& Item = Thresholds[Idx];
		if (DistanceSqrd <= (Item.GetMaxDistSqrd()))
		{
			return static_cast<float>(Item.Significance);
		}
	}
	
	return static_cast<float>(ESignificanceValue::Highest);
}

void UUPSignificanceComponent::UpdateParticleSignificance(float NewSignificance)
{
	// @TODO: Activate/Play resets significance, meaning we can't set the significance if things like the muzzle aren't yet playing.
	
	// Push new required significance to Cascade particles (Niagara to be added later...maybe)
	if (bManageOwnerParticleSignificance)
	{
		// "Low" significance particles are culled first
		EParticleSignificanceLevel CurrSignificance;
		if (NewSignificance == static_cast<float>(ESignificanceValue::Highest))
		{
			// This works inverted from how we defined our own ENUM elsewhere. It's set to "require X significance or we cull this particle"
			// When our significance is very high (close to camera) we don't cull any particles. So the required significance = "Low"
			CurrSignificance = EParticleSignificanceLevel::Low;
		}
		// Lowest significance, only render critical particles
		else if (NewSignificance <= static_cast<float>(ESignificanceValue::Lowest))
		{
			CurrSignificance = EParticleSignificanceLevel::Critical;
		}
		else
		{
			// Keeping things simple, for any significance inbetween we use "medium" (we don't define a range for "High" anywhere in this Example but it will still be rendered the same as Medium)
			CurrSignificance = EParticleSignificanceLevel::Medium;
		}
		
		TArray<UParticleSystemComponent*> Particles;
		GetOwner()->GetComponents<UParticleSystemComponent>(Particles);

		for (UParticleSystemComponent* Comp : Particles)
		{
			Comp->SetRequiredSignificance(CurrSignificance);

			// High "required" significance means we quickly cull many particle emitters that don't match the requirement
			//UE_LOG(LogTemp, Log, TEXT("Changed required particle significance to %s for %s in %s"), *UEnum::GetValueAsString(CurrSignificance), *Comp->GetName(), *GetOwner()->GetName());
		}
	}
}
