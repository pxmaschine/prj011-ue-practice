// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPDashProjectile.h"
#include "Course/UPProjectileMovementComponent.h"
#include "Course/UPPlayerController.h"

#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"


AUPDashProjectile::AUPDashProjectile()
{
	TeleportDelay = 0.2f;
	DetonateDelay = 0.2f;

	MovementComp->InitialSpeed = 6000.0f;
}

void AUPDashProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_DelayedDetonate, this, &AUPDashProjectile::Explode, DetonateDelay);
}

void AUPDashProjectile::Explode_Implementation()
{
	// Clear timer if the Explode was already called through another source like OnActorHit
	GetWorldTimerManager().ClearTimer(TimerHandle_DelayedDetonate);

	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());

	NiagaraLoopComp->Deactivate();

	MovementComp->StopMovementImmediately();
	SetActorEnableCollision(false);

	FTimerHandle TimerHandle_DelayedTeleport;
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedTeleport, this, &AUPDashProjectile::TeleportInstigator, TeleportDelay);

	// Skip base implementation as it will destroy actor (we need to stay alive a bit longer to finish the 2nd timer)
	//Super::Explode_Implementation();
}

void AUPDashProjectile::TeleportInstigator()
{
	AActor* ActorToTeleport = GetInstigator();

	// Must be set in spawning code (also good for things like knowing who caused the damage)
	check(ActorToTeleport);

	// Keep instigator rotation or it may end up jarring
	ActorToTeleport->TeleportTo(GetActorLocation(), ActorToTeleport->GetActorRotation(), false, false);

	// Play shake on the player we teleported
	const APawn* InstigatorPawn = CastChecked<APawn>(ActorToTeleport);
	APlayerController* PC = InstigatorPawn->GetController<AUPPlayerController>();

	// Controller can be nullptr if we died (and detached the pawn) just after launching the dash projectile
	if (PC && PC->IsLocalController())
	{
		PC->ClientStartCameraShake(ImpactShake);
	}

	// Now we're ready to destroy self
	Destroy();
}

