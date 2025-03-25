// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPAction_ProjectileAttack.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


UUPAction_ProjectileAttack::UUPAction_ProjectileAttack()
{
	AttackAnimDelay = 0.2f;
	HandSocketName = "Muzzle_01";

	SweepRadius = 20.0f;
	SweepDistanceFallback = 5000;
}

void UUPAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if (ACharacter* Character = Cast<ACharacter>(Instigator))
	{
		Character->PlayAnimMontage(AttackAnim);

		UGameplayStatics::SpawnEmitterAttached(CastVFX, Character->GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);

		// Only on server
		if (Character->HasAuthority())
		{
			FTimerHandle TimerHandle_AttackDelay;
			FTimerDelegate Delegate;
			Delegate.BindUFunction(this, "AttackDelay_Elapsed", Character);

			GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, AttackAnimDelay, false);
		}
	}
}

void UUPAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	if (ensureAlways(ProjectileClass))
	{
		const FVector HandLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);

		// We trace against the environment first to find whats under the player crosshair.
		// We use the hit location to adjust the projectile launch direction so it will hit what is under the crosshair rather than shoot straight forward from the player hands.

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = InstigatorCharacter;

		FCollisionShape Shape;
		Shape.SetSphere(SweepRadius);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(InstigatorCharacter);

		FVector TraceDirection = InstigatorCharacter->GetControlRotation().Vector();

		// Add sweep radius onto start to avoid the sphere clipping into floor/walls the camera is directly against.
		FVector TraceStart = InstigatorCharacter->GetPawnViewLocation() + (TraceDirection * SweepRadius);
		// endpoint far into the look-at distance (not too far, still adjust somewhat towards crosshair on a miss)
		FVector TraceEnd = TraceStart + (TraceDirection * SweepDistanceFallback);

		FHitResult Hit;
		// returns true if we got to a blocking hit (Channel1="Projectile" defined in DefaultGame.ini)
		if (GetWorld()->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat::Identity, ECC_GameTraceChannel1, Shape, Params))
		{
			TraceEnd = Hit.ImpactPoint;
		}

		FRotator ProjRotation = (TraceEnd - HandLocation).Rotation();
		FTransform SpawnTM = FTransform(ProjRotation, HandLocation);
		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	}

	StopAction(InstigatorCharacter);
}
