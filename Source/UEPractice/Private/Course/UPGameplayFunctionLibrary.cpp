// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPGameplayFunctionLibrary.h"

#include "ShaderPipelineCache.h"
#include "Course/UPAttributeComponent.h"
#include "Kismet/GameplayStatics.h"


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
		if (HitComp->bApplyImpulseOnDamage && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{
			FVector Direction = HitResult.TraceEnd - HitResult.TraceStart;
			Direction.Normalize();

			// @todo: allow configuration for impulse strength
			HitComp->AddImpulseAtLocation(Direction * 30000.f, HitResult.ImpactPoint, HitResult.BoneName);
		}
		return true;
	}

	// Call into Unreal built in logic for early course damaging of explosive barrel
	// Don't have a proper way of knowing it was processed
	UGameplayStatics::ApplyDamage(TargetActor, DamageAmount, nullptr, DamageInstigator, nullptr);

	return false;
}

int32 UUPGameplayFunctionLibrary::GetRemainingBundledPSOs()
{
	// Counts Bundled PSOs remaining, exposed for UI access
	return FShaderPipelineCache::NumPrecompilesRemaining();
}
