// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPExplosiveBarrel.h"
#include "UEPractice/UEPractice.h"

#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UPExplosiveBarrel)

// Sets default values
AUPExplosiveBarrel::AUPExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	RootComponent = StaticMesh;

	RadialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForce->Radius = 700.0f;
	RadialForce->ImpulseStrength = 2500.0f;
	RadialForce->bImpulseVelChange = true;
	RadialForce->SetupAttachment(StaticMesh);

	// Leaving this on applies small constant force via component 'tick'
	RadialForce->SetAutoActivate(false);
	RadialForce->AddCollisionChannelToAffect(ECC_WorldDynamic);

	ExplosionComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ExplosionComp"));
	ExplosionComp->bAutoActivate = false;
	ExplosionComp->SetupAttachment(StaticMesh);
}

float AUPExplosiveBarrel::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	// Could safely skip the base logic...
	//DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	RadialForce->FireImpulse();

	ExplosionComp->Activate();

	// @todo: cause damage to other stuff around it

	UE_LOGFMT(LogGame, Log, "OnActorHit in Explosive Barrel");

	// Warnings as structed logs even show up in the "Message Log" window of UnrealEd

	UE_LOGFMT(LogGame, Warning, "OnActorHit, OtherActor: {name}, at game time: {timeseconds}", GetNameSafe(DamageCauser), GetWorld()->TimeSeconds);

	//FString CombinedString = FString::Printf(TEXT("Hit at location: %s"), *Hit.ImpactPoint.ToString());
	//DrawDebugString(GetWorld(), Hit.ImpactPoint, CombinedString, nullptr, FColor::Green, 2.0f, true);

	return DamageAmount;
}
