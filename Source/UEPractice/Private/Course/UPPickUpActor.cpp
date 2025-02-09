// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPPickUpActor.h"

#include "Components/SphereComponent.h"

// Sets default values
AUPPickUpActor::AUPPickUpActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("PickUp");
	SphereComp->SetSphereRadius(100.0f);
	RootComponent = SphereComp;

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
