// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPGameplayFunctionLibrary.h"
#include "Course/UPActionComponent.h"
#include "Course/UPActionSystemInterface.h"
#include "Course/SharedGameplayTags.h"
#include "UEPractice/UEPractice.h"

#include "ShaderPipelineCache.h"


UUPActionComponent* UUPGameplayFunctionLibrary::GetActionComponentFromActor(const AActor* FromActor)
{
	if (!ensure(FromActor))
	{
		// Log this?
		return nullptr;
	}

	// Note: Cast<T> on interface only works if the interface was implemented on the Actor in C++
	// For BP implemented we should change this code to call Execute_GetActionComponent instead...
	if (const IUPActionSystemInterface* ASI = Cast<IUPActionSystemInterface>(FromActor))
	{
		return ASI->GetActionComponent();
	}
	/*if (InActor && InActor->Implements<URogueGameplayInterface>()) // example reference for a BP interface
	{
		URogueActionComponent* ActionComp = nullptr;
		if (IRogueGameplayInterface::Execute_GetActionComponent(InActor, ActionComp))
		{
			return ActionComp;
		}
	}*/

	// Fallback when interface is missing
	return FromActor->FindComponentByClass<UUPActionComponent>();
}

bool UUPGameplayFunctionLibrary::IsAlive(const AActor* InActor)
{
	if (!IsValid(InActor))
	{
		return false;
	}

	if (UUPActionComponent* ActionComp = GetActionComponentFromActor(InActor))
	{
		return ActionComp->GetAttribute(SharedGameplayTags::Attribute_Health)->GetValue() > 0.0f;
	}

	return false;
}

bool UUPGameplayFunctionLibrary::IsFullHealth(AActor* InActor)
{
	UUPActionComponent* ActionComp = UUPActionComponent::GetActionComponent(InActor);

	const FUPAttribute* HealthAttribute = ActionComp->GetAttribute(SharedGameplayTags::Attribute_Health);
	const FUPAttribute* HealthMaxAttribute = ActionComp->GetAttribute(SharedGameplayTags::Attribute_HealthMax);

	return HealthAttribute->GetValue() >= HealthMaxAttribute->GetValue();
}

/*
 	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f)
	{
		return false;
	}

	if (Delta < 0.0f)
	{
		const float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();
		Delta *= DamageMultiplier;
	}

	const float OldHealth = Health;
	const float NewHealth = FMath::Clamp(Health + Delta, 0.0f, MaxHealth);

	const float ActualDelta = NewHealth - OldHealth;

	// Is Server?
	if (GetOwner()->HasAuthority())
	{
		Health = NewHealth;

		if (!FMath::IsNearlyZero(ActualDelta))
		{
			MulticastHealthChanged(InstigatorActor, Health, ActualDelta);
		}

		// Died
		if (ActualDelta < 0.0f && FMath::IsNearlyZero(Health))
		{
			if (AUPGameModeBase* GM = GetWorld()->GetAuthGameMode<AUPGameModeBase>())
			{
				GM->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
	}

	return !FMath::IsNearlyZero(ActualDelta);
 */

bool UUPGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageCoefficient)
{
	UUPActionComponent* InstigatorComp = GetActionComponentFromActor(DamageCauser);
	check(InstigatorComp);

	const FUPAttribute* FoundAttribute = InstigatorComp->GetAttribute(SharedGameplayTags::Attribute_AttackDamage);

	// Coefficient is a %, to scale all out damage off the instigator's base attack damage
	float TotalDamage = FoundAttribute->GetValue() * (DamageCoefficient*0.01f);

	UUPActionComponent* VictimComp = GetActionComponentFromActor(TargetActor);
	if (VictimComp == nullptr)
	{
		UE_LOG(LogGame, Warning, TEXT("ApplyDamage Victim (%s) does not contain an ActionComponent."), *GetNameSafe(TargetActor));
		return false;
	}

	FAttributeModification AttriMod = FAttributeModification(
		SharedGameplayTags::Attribute_Health,
		-TotalDamage, // Make sure we apply a negative amount to the Health
		VictimComp,
		DamageCauser,
		EAttributeModifyType::AddDelta);
	
	// Could pass through dead enemies
	return VictimComp->ApplyAttributeChange(AttriMod);
}

bool UUPGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageCoefficient, const FHitResult& HitResult)
{
	if (ApplyDamage(DamageCauser, TargetActor, DamageCoefficient))
	{
		UPrimitiveComponent* HitComp = HitResult.GetComponent();
		if (HitComp->bApplyImpulseOnDamage && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{
			// Direction = Target - Origin
			FVector Direction = HitResult.TraceEnd - HitResult.TraceStart;
			Direction.Normalize();

			// @todo: allow configuration for impulse strength
			HitComp->AddImpulseAtLocation(Direction * 30000.f, HitResult.ImpactPoint, HitResult.BoneName);
		}
		return true;
	}

	// Call into Unreal built in logic for early course damaging of explosive barrel
	// Don't have a proper way of knowing it was processed, so we return false
	//UGameplayStatics::ApplyDamage(TargetActor, DamageAmount, nullptr, DamageCauser, nullptr);

	return false;
}

/*
bool URogueGameplayFunctionLibrary::ApplyRadialDamage(AActor* DamageCauser, FVector Origin, float DamageRadius, float DamageCoefficient)
{
	UWorld* World = DamageCauser->GetWorld();
	// do async overlap to find list of potential victims
	// only test for actors with action component / or gameobject interface
	// 2nd pass is another async trace for occlusion tests (optional)
	// GameObject interface: GetDamageTraceLocations(TArray<FVector>& OutLocations);
	// allow objects or pawns to specify which locations they want to use for occlusion tests, for example
	// head, spine, hands, legs on a character
	// for large and oddly shapes objects, it can also use "nearest collision point" from origin rather than the actor location

	FCollisionShape Shape;
	Shape.SetSphere(DamageRadius);

	FCollisionQueryParams Params;
	//Params.MobilityType = EQueryMobilityType::Dynamic;

	FCollisionResponseParams ResponseParams;
*/

	/*
	FOverlapDelegate* Delegate;
	Delegate->BindLambda([](const FTraceHandle& Handle, FOverlapDatum& Datum)
		{
			// ... called when ready
		});*/

	//FTraceHandle Handle; // @todo: can pass in additional params here if needed for multi-pass stuff
	// @todo:need to pass "this", which wont work in static function

/*
	// Fill any useful dmg info
	FDamageInfo Info;
	Info.DamageInstigator = DamageCauser;

	Info.AttackDamage = 0.0f; // InstigatorDmg * (DamageCoefficient*0.01f)
	
	FOverlapDelegate Delegate = FOverlapDelegate::CreateUObject(this, &URogueGameplayFunctionLibrary::OnDamageOverlapComplete, Info);

	World->AsyncOverlapByChannel(Origin, FQuat::Identity, COLLISION_PROJECTILE,
		Shape, Params, ResponseParams, &Delegate);

	return false;
}
*/

/*	
void URogueGameplayFunctionLibrary::OnDamageOverlapComplete(const FTraceHandle& TraceHandle, FOverlapDatum& OverlapDatum, FDamageInfo DamageInfo)
{
	// if second pass w/ line traces is async too, we are two frames 'behind' the initial request for damage.

	// @todo: iterate the victims to apply damage: OverlapDatum.OutOverlaps

	check(DamageInfo.DamageInstigator.Get());

	for (FOverlapResult& Overlap : OverlapDatum.OutOverlaps)
	{
		ApplyDamage(DamageInfo.DamageInstigator.Get(), Overlap.GetActor(), DamageInfo.AttackDamage);
	}
}
*/

int32 UUPGameplayFunctionLibrary::GetRemainingBundledPSOs()
{
	// Counts Bundled PSOs remaining, exposed for UI access
	return FShaderPipelineCache::NumPrecompilesRemaining();
}
