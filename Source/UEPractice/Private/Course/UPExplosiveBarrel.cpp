// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPExplosiveBarrel.h"
#include "Course/UPAttributeComponent.h"
#include "UEPractice/UEPractice.h"

#include "NiagaraComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UPExplosiveBarrel)

// Sets default values
AUPExplosiveBarrel::AUPExplosiveBarrel()
{
	AttributeComponent = CreateDefaultSubobject<UUPAttributeComponent>(TEXT("AttributeComp"));
	AttributeComponent->OnHealthChanged.AddDynamic(this, &ThisClass::OnHealthChanged);
	
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

	ExplosionComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ExplosionComp"));
	ExplosionComp->bAutoActivate = false;
	ExplosionComp->SetupAttachment(StaticMesh);

	FlamesFXComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FlamesFXComp"));
	FlamesFXComp->bAutoActivate = false;
	FlamesFXComp->SetupAttachment(StaticMesh);

	ExplosionDelayTime = 2.0f;
}

void AUPExplosiveBarrel::OnHealthChanged(AActor* InstigatorActor, UUPAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if (bExploded)
	{
		return;
	}
	
	// Either use a hit counter or 'hitpoints' (For early assignment, we don't have any kind of attributes yet)
	HitCounter++;

	if (HitCounter == 1)
	{
		FlamesFXComp->Activate(true);

		GetWorldTimerManager().SetTimer(DelayedExplosionHandle, this, &ThisClass::Explode, ExplosionDelayTime);
	}
	else if (HitCounter == 2)
	{
		// Avoid second explosion later
		GetWorldTimerManager().ClearTimer(DelayedExplosionHandle);
		
		Explode();
	}
	
	// Structured Logging Example
	UE_LOGFMT(LogGame, Log, "OnActorHit in Explosive Barrel");
	// Warnings as structured logs even show up in the "Message Log" window of UnrealEd
	UE_LOGFMT(LogGame, Warning, "OnActorHit, OtherActor: {name}, at game time: {timeseconds}", GetNameSafe(InstigatorActor), GetWorld()->TimeSeconds);
}

/*
float AUPExplosiveBarrel::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                     AActor* DamageCauser)
{
	
	return DamageAmount;
}
*/

void AUPExplosiveBarrel::Explode()
{
	if (bExploded)
	{
		// Nothing to do here
		return;
	}

	bExploded = true;

	FlamesFXComp->Deactivate();

	RadialForce->FireImpulse();

	ExplosionComp->Activate();

	// @todo: cause damage to other stuff around it

	//FString CombinedString = FString::Printf(TEXT("Hit at location: %s"), *Hit.ImpactPoint.ToString());
	//DrawDebugString(GetWorld(), Hit.ImpactPoint, CombinedString, nullptr, FColor::Green, 2.0f, true);
}
