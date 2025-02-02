// Fill out your copyright notice in the Description page of Project Settings.


#include "UPMagicProjectile.h"

#include "UPAttributeComponent.h"
#include "Components/SphereComponent.h"

void AUPMagicProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AUPMagicProjectile::OnActorOverlap);
}

void AUPMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		UUPAttributeComponent* AttributeComp = Cast<UUPAttributeComponent>(OtherActor->GetComponentByClass(UUPAttributeComponent::StaticClass()));
		if (AttributeComp)
		{
			AttributeComp->ApplyHealthChange(-20.0f);

			Destroy();
		}
	}
}
