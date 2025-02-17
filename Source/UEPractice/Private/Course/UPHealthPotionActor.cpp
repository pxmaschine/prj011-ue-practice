// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPHealthPotionActor.h"
#include "Course/UPAttributeComponent.h"

AUPHealthPotionActor::AUPHealthPotionActor()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	// Disable collision, instead we use SphereComp to handle interaction queries
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetupAttachment(RootComponent);
}

void AUPHealthPotionActor::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn))
	{
		return;
	}

	UUPAttributeComponent* AttributeComp = Cast<UUPAttributeComponent>(InstigatorPawn->GetComponentByClass(UUPAttributeComponent::StaticClass()));
	// Check if not already at max health
	if (ensure(AttributeComp) && !AttributeComp->IsFullHealth())
	{
		// Only activate if healed successfully
		if (AttributeComp->ApplyHealthChange(this, AttributeComp->GetMaxHealth()))
		{
			HideAndCooldownPickUp();
		}
	}
}
