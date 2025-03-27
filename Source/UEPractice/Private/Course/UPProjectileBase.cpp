// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPProjectileBase.h"
#include "Course/UPProjectileMovementComponent.h"
#include "Course/UPActorPoolingSubsystem.h"

#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
AUPProjectileBase::AUPProjectileBase()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Projectile");
	// Don't bother telling the nav system whenever we move
	SphereComp->SetCanEverAffectNavigation(false);
	RootComponent = SphereComp;

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);

	AudioComp = CreateDefaultSubobject<UAudioComponent>("AudioComp");
	AudioComp->SetupAttachment(RootComponent);

	// Custom Projectile Component (for tick management & better homing)
	MovementComp = CreateDefaultSubobject<UUPProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 8000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;
	MovementComp->ProjectileGravityScale = 0.0f;

	ImpactShakeInnerRadius = 0.0f;
	ImpactShakeOuterRadius = 1500.0f;

	bReplicates = true;
}

void AUPProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// More consistent to bind here compared to Constructor which may fail to bind if Blueprint was created before adding this binding (or when using hotreload)
	// PostInitializeComponent is the preferred way of binding any events.
	SphereComp->OnComponentHit.AddDynamic(this, &AUPProjectileBase::OnActorHit);
}

void AUPProjectileBase::BeginPlay()
{
	Super::BeginPlay();
}

void AUPProjectileBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Prepare for pool by disabling and resetting for the next time it will be re-used
	{
		// Complete any active PSCs (These may be pooled themselves by the particle manager) - old Cascade system since we dont use Niagara yet
		TInlineComponentArray<UParticleSystemComponent*> ParticleComponents(this);
		for (UParticleSystemComponent* const PSC  : ParticleComponents)
		{
			PSC->Complete();
		}
	}
}

void AUPProjectileBase::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Explode();
}

void AUPProjectileBase::LifeSpanExpired()
{
	//Super::LifeSpanExpired();

	// Skip destroy and instead release to pool
	// This would need to be generically implemented for any pooled actor
	UUPActorPoolingSubsystem* PoolingSubsystem = GetWorld()->GetSubsystem<UUPActorPoolingSubsystem>();
	PoolingSubsystem->ReleaseToPool(this);
}

void AUPProjectileBase::Explode_Implementation()
{
	// Check to make sure we aren't already being destroyed
	if (ensure(IsValid(this)))
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		UGameplayStatics::PlayWorldCameraShake(this, ImpactShake, GetActorLocation(), ImpactShakeInnerRadius, ImpactShakeOuterRadius);

		//Destroy();
		// Release back to pool instead of destroying
		UUPActorPoolingSubsystem* PoolingSubsystem = GetWorld()->GetSubsystem<UUPActorPoolingSubsystem>();
		PoolingSubsystem->ReleaseToPool(this);
	}
}
