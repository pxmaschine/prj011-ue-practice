// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPExplosiveBarrel.h"
#include "Course/UPAttributeComponent.h"
#include "UEPractice/UEPractice.h"

#include "PhysicsEngine/RadialForceComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UPExplosiveBarrel)

// Sets default values
AUPExplosiveBarrel::AUPExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	RootComponent = StaticMesh;

	RadialForce = CreateDefaultSubobject<URadialForceComponent>("RadialForce");
	RadialForce->Radius = 700.0f;
	RadialForce->ImpulseStrength = 2500.0f;
	RadialForce->bImpulseVelChange = true;
	RadialForce->SetupAttachment(StaticMesh);

	// Leaving this on applies small constant force via component 'tick'
	RadialForce->SetAutoActivate(false);
	RadialForce->AddCollisionChannelToAffect(ECC_WorldDynamic);
}

void AUPExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	/*
	 * TODO: This fails Ensure condition. Why?
	 *   Ensure condition failed: InvocationList[ CurFunctionIndex ] != InDelegate 
	 */
	//StaticMesh->OnComponentHit.AddDynamic(this, &AUPExplosiveBarrel::OnHitCallback);

	// AddUniqueDynamic is used here, because we previously had it in the constructor and it was built into the blueprint asset as such
	// That's why it throws an error on startup
	StaticMesh->OnComponentHit.AddUniqueDynamic(this, &AUPExplosiveBarrel::OnHitCallback);
}

void AUPExplosiveBarrel::OnHitCallback(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	RadialForce->FireImpulse();

	UE_LOGFMT(LogGame, Log, "OnActorHit in Explosive Barrel");

	// Warnings as structed logs even show up in the "Message Log" window of UnrealEd
	UE_LOGFMT(LogGame, Warning, "OnActorHit, OtherActor: {name}, at game time: {timeseconds}", GetNameSafe(OtherActor), GetWorld()->TimeSeconds);

	FString CombinedString = FString::Printf(TEXT("Hit at location: %s"), *Hit.ImpactPoint.ToString());
	DrawDebugString(GetWorld(), Hit.ImpactPoint, CombinedString, nullptr, FColor::Green, 2.0f, true);

	if (OtherActor)
	{
		UUPAttributeComponent* AttributeComp = Cast<UUPAttributeComponent>(OtherActor->GetComponentByClass(UUPAttributeComponent::StaticClass()));
		if (AttributeComp)
		{
			AttributeComp->ApplyHealthChange(this, -50.0f);
		}
	}
}
