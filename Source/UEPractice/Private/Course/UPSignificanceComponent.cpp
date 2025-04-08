// Fill out your copyright notice in the Description page of Project Settings.

#include "Course/UPSignificanceComponent.h"
#include "Course/UPSignificanceInterface.h"

#include "NiagaraComponent.h"


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
	bHasImplementedInterface = false;
	bInsignificantWhenOwnerIsHidden = true;
	bManageOwnerParticleSignificance = true;
	CurrentSignificance = ESignificanceValue::Invalid;

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

		// Manage particle components
		if (bManageOwnerParticleSignificance)
		{
			// @todo: implement niagara FX
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

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

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
		// for 'EPostSignificanceType::Concurrent' you need 'thread safe' post significance function
		// our sigman update runs during the game viewport update tick, so it should *probably* be ok so long as no other non-GT logic is interacting with the objects.
		const FName Tag = GetOwner()->GetClass()->GetFName();
		SignificanceManager->RegisterObject(this, Tag, SignificanceFunc, USignificanceManager::EPostSignificanceType::Concurrent, PostSignificanceFunc);
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
		// disabled to run cascade update every frame (for now)
		//return;
	}

	if (Significance != OldSignificance || CurrentSignificance == ESignificanceValue::Invalid)
	{
		CurrentSignificance = static_cast<ESignificanceValue>(Significance);
		UE_LOG(LogTemp, Log, TEXT("Significance for %s changed to %s"), *GetNameSafe(GetOwner()), *UEnum::GetValueAsString(CurrentSignificance));
		OnSignificanceChanged.Broadcast(CurrentSignificance);
	}

	// Running this every frame for now (commented out the above early out) as otherwise we can't properly cull the cascade VFX (to be re-worked later)
	if (!bManageOwnerParticleSignificance)
	{
		UpdateParticleSignificance(Significance);
	}
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
	// Niagara Particle Systems
	// @todo: we don't need to call into niagara, the EffectType significance handler can do this for us...
	/*TArray<UNiagaraComponent*> NiagaraSystems;
	GetOwner()->GetComponents<UNiagaraComponent>(NiagaraSystems);

	for (UNiagaraComponent* Comp : NiagaraSystems)
	{
		// Niagara uses 'int32 index' to set significance, you should map this with the input "float NewSignificance" (eg. not something between 0.0-1.0 as it gets rounded)
		
		Comp->SetSystemSignificanceIndex(NewSignificance);
	}*/
}
