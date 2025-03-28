// Fill out your copyright notice in the Description page of Project Settings.

#include "Course/UPTargetDummy.h"
#include "Course/UPAttributeComponent.h"

// Sets default values
AUPTargetDummy::AUPTargetDummy()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	AttributeComp = CreateDefaultSubobject<UUPAttributeComponent>(TEXT("AttributeComp"));
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
