// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPPickUpActor.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AUPPickUpActor::AUPPickUpActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("PickUp");
	SphereComp->SetSphereRadius(100.0f);
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	// Disable collision, instead we use SphereComp to handle interaction queries
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetupAttachment(RootComponent);

	RespawnTime = 10.0f;
}

void AUPPickUpActor::ShowPickUp()
{
	SetPickUpState(true);
}

void AUPPickUpActor::HideAndCooldownPickUp()
{
	SetPickUpState(false);

	FTimerHandle TimerHandle_RespawnTimer;
	GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &AUPPickUpActor::ShowPickUp, RespawnTime);
}

void AUPPickUpActor::SetPickUpState(bool bNewIsActive)
{
	SetActorEnableCollision(bNewIsActive);

	// Set visibility on root and all children
	RootComponent->SetVisibility(bNewIsActive, true);
}
