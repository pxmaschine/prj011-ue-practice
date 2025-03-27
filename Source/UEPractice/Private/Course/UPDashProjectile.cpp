// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPDashProjectile.h"
#include "Course/UPProjectileMovementComponent.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"


AUPDashProjectile::AUPDashProjectile()
{
	TeleportDelay = 0.2f;
	DetonateDelay = 0.2f;

	InitialLifeSpan = 5.0f;

	MovementComp->InitialSpeed = 6000.0f;
}

void AUPDashProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_DelayedDetonate, this, &AUPDashProjectile::Explode, DetonateDelay);
}

void AUPDashProjectile::Explode_Implementation()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_DelayedDetonate);

	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

	NiagaraLoopComp->Deactivate();

	MovementComp->StopMovementImmediately();
	SetActorEnableCollision(false);

	// TODO: Play sound cue
	//ExampleContent/Audio/ITEM/ITEM_Cloud_Spawn_Cue

	FTimerHandle TimerHandle_DelayedTeleport;
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedTeleport, this, &AUPDashProjectile::TeleportInstigator, TeleportDelay);
}

void AUPDashProjectile::TeleportInstigator()
{
	AActor* ActorToTeleport = GetInstigator();
	if (ensure(ActorToTeleport))
	{
		// TODO
		// Specifically this line would return false in bool AActor::TeleportTo
		// bTeleportSucceeded = MyWorld->FindTeleportSpot(this, NewLocation, DestRotation);
		ActorToTeleport->TeleportTo(GetActorLocation(), ActorToTeleport->GetActorRotation(), false, false);
	}
}

