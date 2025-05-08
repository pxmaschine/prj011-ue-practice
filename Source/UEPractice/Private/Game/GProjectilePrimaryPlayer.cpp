// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GProjectilePrimaryPlayer.h"
#include <Game/GAICharacter.h>
#include <Course/UPGameplayFunctionLibrary.h>
#include <Course/UPProjectileMovementComponent.h>

#include "Components/SphereComponent.h"


AGProjectilePrimaryPlayer::AGProjectilePrimaryPlayer()
{
	SphereComp->SetSphereRadius(20.0f);

	InitialLifeSpan = 8.0f;
	ImpulseStrength = 30000.f;

	DamageCoefficient = 100.0f;
	HomingAcceleration = 5000.0f;
}

void AGProjectilePrimaryPlayer::SetTarget(AActor* TargetActor)
{
	MovementComp->HomingTargetComponent = TargetActor->GetRootComponent();
	MovementComp->HomingAccelerationMagnitude = HomingAcceleration;
	MovementComp->bIsHomingProjectile = true;
}

void AGProjectilePrimaryPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AGProjectilePrimaryPlayer::OnActorOverlap);
}

void AGProjectilePrimaryPlayer::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetInstigator())
	{
		return;
	}

	//if (AGAICharacter* AI = Cast<AGAICharacter>(OtherActor))
	//{
	//	AI->Kill();

	//	// Direction = Target - Origin
	//	FVector Direction = SweepResult.TraceEnd - SweepResult.TraceStart;
	//	Direction.Normalize();

	//	UPrimitiveComponent* HitComp = SweepResult.GetComponent();
	//	HitComp->AddImpulseAtLocation(Direction * ImpulseStrength, SweepResult.ImpactPoint, SweepResult.BoneName);

	//	Explode();
	//}

	//// Parry Ability (GameplayTag Example)
	//UUPActionComponent* OtherActionComp = OtherActor->FindComponentByClass<UUPActionComponent>();
	//if (OtherActionComp && OtherActionComp->ActiveGameplayTags.HasTag(ParryTag))
	//{
	//	MovementComp->Velocity = -MovementComp->Velocity;

	//	// The reflector now becomes the 'instigator' of the damage from the reflected projectile
	//	SetInstigator(Cast<APawn>(OtherActor));
	//	return;
	//}

	// Apply Damage & Impulse
	if (UUPGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageCoefficient, SweepResult, ImpulseStrength))
	{
		// We only explode if the target can be damaged, it ignores anything it Overlaps that it cannot Damage
		Explode();

		//if (OtherActionComp && BurningActionClass && HasAuthority())
		//{
		//	OtherActionComp->AddAction(GetInstigator(), BurningActionClass);
		//}
	}
}
