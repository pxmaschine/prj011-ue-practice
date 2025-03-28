// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPPickUpActor.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AUPPickUpActor::AUPPickUpActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionProfileName("PickUp");
	SphereComp->SetSphereRadius(100.0f);
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	// Disable collision, instead we use SphereComp to handle interaction queries
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetupAttachment(RootComponent);

	RespawnTime = 10.0f;
	bIsPickedUp = false;

	bReplicates = true;
}

void AUPPickUpActor::Interact_Implementation(APawn* InstigatorPawn)
{
	// Implemented in derived classes
}

FText AUPPickUpActor::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	return FText::GetEmpty();
}

void AUPPickUpActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUPPickUpActor, bIsPickedUp);
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
	bIsPickedUp = !bNewIsActive;

	OnRep_PickedUp();
}

void AUPPickUpActor::OnRep_PickedUp()
{
	SetActorEnableCollision(!bIsPickedUp);

	// Set visibility on root and all children
	RootComponent->SetVisibility(!bIsPickedUp, true);
}
