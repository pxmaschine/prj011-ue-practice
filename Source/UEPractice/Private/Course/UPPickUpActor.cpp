// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPPickUpActor.h"
#include "UEPractice/UEPractice.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AUPPickUpActor::AUPPickUpActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionProfileName(Collision::Pickup_ProfileName);
	SphereComp->SetSphereRadius(100.0f);
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	// Disable collision, instead we use SphereComp to handle interaction queries
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetupAttachment(RootComponent);

	RespawnTime = 10.0f;
	bIsActive = true;

	bReplicates = true;
}

void AUPPickUpActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Some pickups should auto pickup on overlap rather than a choice through player input
	if (bCanAutoPickup)
	{
		SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
	}
}

void AUPPickUpActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearAllTimersForObject(this);
}

void AUPPickUpActor::Interact_Implementation(APawn* InstigatorPawn)
{
	// Implemented in derived classes
}

FText AUPPickUpActor::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	return FText::GetEmpty();
}

void AUPPickUpActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// @todo: prevent minions from pickup coins. GameplayTags or collision channel.
	Execute_Interact(this, CastChecked<APawn>(OtherActor));
}

void AUPPickUpActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUPPickUpActor, bIsActive);
}

void AUPPickUpActor::ShowPickUp()
{
	SetPowerupState(true);
}

void AUPPickUpActor::HideAndCooldownPickUp()
{
	SetPowerupState(false);

	FTimerHandle TimerHandle_RespawnTimer;
	GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &AUPPickUpActor::ShowPickUp, RespawnTime);
}

void AUPPickUpActor::SetPowerupState(bool bNewIsActive)
{
	bIsActive = bNewIsActive;

	OnRep_PickedUp();
}

void AUPPickUpActor::OnRep_PickedUp()
{
	SetActorEnableCollision(bIsActive);

	// Set visibility on root and all children
	RootComponent->SetVisibility(bIsActive, true);
}
