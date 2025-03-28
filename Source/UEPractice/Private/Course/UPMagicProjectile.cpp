// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPMagicProjectile.h"
#include "Course/UPActionComponent.h"
#include "Course/UPGameplayFunctionLibrary.h"
#include "Course/UPProjectileMovementComponent.h"
#include "Course/UPActionEffect.h"

#include "Components/SphereComponent.h"


AUPMagicProjectile::AUPMagicProjectile()
{
	SphereComp->SetSphereRadius(20.0f);

	InitialLifeSpan = 8.0f;

	DamageAmount = 20.0f;
}

void AUPMagicProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AUPMagicProjectile::OnActorOverlap);
}

void AUPMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetInstigator())
	{
		return;
	}

	// Parry Ability (GameplayTag Example)
	UUPActionComponent* ActionComp = OtherActor->FindComponentByClass<UUPActionComponent>();
	if (ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
	{
		MovementComp->Velocity = -MovementComp->Velocity;

		SetInstigator(Cast<APawn>(OtherActor));

		return;
	}

	if (UUPGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageAmount, SweepResult))
	{
		Explode();

		if (ActionComp && HasAuthority())
		{
			ActionComp->AddAction(GetInstigator(), BurningActionClass);
		}
	}
}
