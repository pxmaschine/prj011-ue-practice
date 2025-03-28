// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPGameplayFunctionLibrary.h"

#include "ShaderPipelineCache.h"
#include "Course/UPAttributeComponent.h"


bool UUPGameplayFunctionLibrary::ApplyDamage(AActor* DamageInstigator, AActor* TargetActor, float DamageAmount)
{
	if (UUPAttributeComponent* AttributeComponent = UUPAttributeComponent::GetAttributes(TargetActor))
	{
		return AttributeComponent->ApplyHealthChange(DamageInstigator, -DamageAmount);
	}

	return false;
}

bool UUPGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageInstigator, AActor* TargetActor,
	float DamageAmount, const FHitResult& HitResult)
{
	if (ApplyDamage(DamageInstigator, TargetActor, DamageAmount))
	{
		UPrimitiveComponent* HitComp = HitResult.GetComponent();
		if (HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{
			FVector Direction = HitResult.TraceEnd - HitResult.TraceStart;
			Direction.Normalize();

			HitComp->AddImpulseAtLocation(Direction * 300000.0f, HitResult.ImpactPoint, HitResult.BoneName);
		}
		return true;
	}

	return false;
}

int32 UUPGameplayFunctionLibrary::GetRemainingBundledPSOs()
{
	// Counts Bundled PSOs remaining, exposed for UI access
	return FShaderPipelineCache::NumPrecompilesRemaining();
}
