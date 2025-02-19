// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPMagicProjectile.h"
#include "Course/UPAttributeComponent.h"

#include "Components/SphereComponent.h"
#include "Course/UPGameplayFunctionLibrary.h"


AUPMagicProjectile::AUPMagicProjectile()
{
	SphereComp->SetSphereRadius(20.0f);

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
	//if (OtherActor && OtherActor != GetInstigator())
	//{
	//	if (UUPAttributeComponent* AttributeComp = Cast<UUPAttributeComponent>(OtherActor->GetComponentByClass(UUPAttributeComponent::StaticClass())))
	//	{
	//		AttributeComp->ApplyHealthChange(GetInstigator(), -DamageAmount);
	//	}
	//	Explode_Implementation();
	//}

	if (!OtherActor || OtherActor == GetInstigator())
	{
		return;
	}

	if (UUPGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageAmount, SweepResult))
	{
		Explode();
	}
}
