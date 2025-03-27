// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPProjectileBase.h"
#include "Course/UPProjectileMovementComponent.h"
#include "Course/UPActorPoolingSubsystem.h"

#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
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

	NiagaraLoopComp = CreateDefaultSubobject<UNiagaraComponent>("EffectComp");
	//NiagaraLoopComp->PoolingMethod = ENCPoolMethod::AutoRelease;
	NiagaraLoopComp->SetupAttachment(RootComponent);

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
}

void AUPProjectileBase::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Explode();
}

void AUPProjectileBase::LifeSpanExpired()
{
	// Skip destroy and instead release to pool

	UUPActorPoolingSubsystem::ReleaseToPool(this);
}

void AUPProjectileBase::PoolBeginPlay_Implementation()
{
	MovementComp->Reset();

	//NiagaraLoopComp->Activate();
	//AudioComp->Play();
	
	// Unpausing is significantly faster than re-creating renderstates due to Deactivate()
	// Does keep its state around which is OK for our loopable VFX that will mostly be active/in-use
	// @todo: ribbon VFX will teleport and cause a large streak on screen as it catches up with the new actor location
	NiagaraLoopComp->SetPaused(false);
	// Reset to fix ribbon positions
	//NiagaraLoopComp->ResetSystem();
	AudioComp->SetPaused(false);
}

void AUPProjectileBase::PoolEndPlay_Implementation()
{
	//NiagaraLoopComp->Deactivate();
	//AudioComp->Stop();
	NiagaraLoopComp->SetPaused(true);
	AudioComp->SetPaused(true);
}

void AUPProjectileBase::Explode_Implementation()
{
	// Check to make sure we aren't already being destroyed
	if (ensure(IsValid(this)))
	{
		// Auto-managed particle pooling
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation(), true, EPSCPoolMethod::AutoRelease);
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		UGameplayStatics::PlayWorldCameraShake(this, ImpactShake, GetActorLocation(), ImpactShakeInnerRadius, ImpactShakeOuterRadius);

		//Destroy();
		// Release back to pool instead of destroying
		UUPActorPoolingSubsystem* PoolingSubsystem = GetWorld()->GetSubsystem<UUPActorPoolingSubsystem>();
		PoolingSubsystem->ReleaseToPool(this);
	}
}
