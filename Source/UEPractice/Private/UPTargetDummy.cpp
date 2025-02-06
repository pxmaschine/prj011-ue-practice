// Fill out your copyright notice in the Description page of Project Settings.

#include "UPTargetDummy.h"
#include "UPAttributeComponent.h"

// Sets default values
AUPTargetDummy::AUPTargetDummy()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;

	AttributeComp = CreateDefaultSubobject<UUPAttributeComponent>("AttributeComp");
}

void AUPTargetDummy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &AUPTargetDummy::OnHealthChanged);
}

void AUPTargetDummy::OnHealthChanged(AActor* InstigatorActor, UUPAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)
	{
		MeshComp->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
	}
}
