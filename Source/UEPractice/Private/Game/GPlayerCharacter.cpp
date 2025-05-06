// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GPlayerCharacter.h"
#include <Game/GGameplayFunctionLibrary.h>

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include <NiagaraFunctionLibrary.h>
#include <UEPractice/UEPractice.h>
#include <Course/UPActorPoolingSubsystem.h>

// Sets default values
AGPlayerCharacter::AGPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = false;
	SpringArmComp->SetupAttachment(RootComponent);
	// We control the rotation of spring arm with pawn control rotation already, this disables a subtle side effect
	// where rotating our CapsuleComponent (eg. caused by bOrientRotationToMovement in Character Movement) will rotate our spring arm until it self corrects later in the update
	// This may cause unwanted effects when using CameraLocation during Tick as it may be slightly offset from our final camera position.
	SpringArmComp->SetUsingAbsoluteRotation(true);
	SpringArmComp->bInheritPitch = false;
	SpringArmComp->bInheritYaw = false;
	SpringArmComp->bInheritRoll = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	UCharacterMovementComponent* CharMoveComp = GetCharacterMovement();
	CharMoveComp->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;

	// Enabled on mesh to react to incoming projectiles
	GetMesh()->SetGenerateOverlapEvents(true);

	// Skip performing overlap queries on the Physics Asset after animation (7 queries in case of our Gideon mesh)
	GetMesh()->bUpdateOverlapsOnAnimationFinalize = false;

	// Disable on capsule collision to avoid double-dipping and receiving 2 overlaps when entering trigger zones etc.
	// Once from the mesh, and 2nd time from capsule
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	RotationSpeed = 10.0f;
}

// Called when the game starts or when spawned
void AGPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AActor* Target = UGGameplayFunctionLibrary::GetClosestEnemy(this);
	if (Target)
	{
		// Get direction to target
		FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
		ToTarget.Z = 0.0f;

		// Smoothly rotate towards the target
		FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), ToTarget.Rotation(), DeltaTime, RotationSpeed);

		//// Remove pitch and roll to keep the character upright
		//NewRotation.Pitch = 0.0f;
		//NewRotation.Roll = 0.0f;

		// Apply new rotation
		Controller->SetControlRotation(NewRotation);
	}

	// -- Rotation Visualization -- //
	constexpr float DrawScale = 100.0f;
	constexpr float Thickness = 5.0f;

	FVector LineStart = GetActorLocation();
	// Offset to the right of pawn
	LineStart += GetActorRightVector() * 100.0f;
	// Set line end in direction of the actor's forward
	FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);
	// Draw Actor's Direction
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0, Thickness);

	FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);
	// Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character)
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.0f, 0, Thickness);
}

// Called to bind functionality to input
void AGPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	const APlayerController* PC = GetController<APlayerController>();
	const ULocalPlayer* LP = PC->GetLocalPlayer();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	// Add mappings for our game, more complex games may have multiple Contexts that are added/removed at runtime
	Subsystem->AddMappingContext(DefaultMappingContext, 0);

	UEnhancedInputComponent* InputComp = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	// General
	InputComp->BindAction(Input_Move, ETriggerEvent::Triggered, this, &AGPlayerCharacter::Move);

	InputComp->BindAction(Input_PrimaryAttack, ETriggerEvent::Triggered, this, &AGPlayerCharacter::PrimaryAttack);

	//// MKB
	//InputComp->BindAction(Input_LookMouse, ETriggerEvent::Triggered, this, &AGPlayerCharacter::LookMouse);
}

//void AGPlayerCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
//{
//	if (CameraComp)
//	{
//		OutLocation = CameraComp->GetComponentLocation();
//		OutRotation = CameraComp->GetComponentRotation();
//	}
//	else
//	{
//		Super::GetActorEyesViewPoint(OutLocation, OutRotation);
//	}
//}
//
//FVector AGPlayerCharacter::GetPawnViewLocation() const
//{
//	return CameraComp->GetComponentLocation();
//}

void AGPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D InputValue = Value.Get<FVector2D>();
	if (!InputValue.IsNearlyZero())
	{
		InputValue = InputValue.GetSafeNormal();
	}

	//FRotator ControlRot = GetControlRotation();
	//ControlRot.Pitch = 0.0f;
	//ControlRot.Roll = 0.0f;

	//AddMovementInput(ControlRot.Vector(), InputValue.Y);
	//AddMovementInput(FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y), InputValue.X);

	AddMovementInput(FVector::ForwardVector, InputValue.Y);
	AddMovementInput(FVector::RightVector, InputValue.X);
}

void AGPlayerCharacter::PrimaryAttack()
{
	PlayAnimMontage(AttackAnim);

	// Auto-released particle pooling
	UNiagaraFunctionLibrary::SpawnSystemAttached(CastVFX, GetMesh(), ProjectileSocketName, FVector::ZeroVector, FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget, true, true, ENCPoolMethod::AutoRelease);

	//UGameplayStatics::SpawnSoundAttached(CastingSound, Character->GetMesh());
	// Alternative to spawning fresh instances for short-lived attacks every time (via SpawnSoundAttached above)
	// we use a single audio component on the player, which uses AutoManageAttachment to detach itself when not active
	PlayAttackSound(CastingSound);

	// Only on server
	if (HasAuthority())
	{
		FTimerHandle TimerHandle_AttackDelay;
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &AGPlayerCharacter::AttackDelay_Elapsed);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, AttackAnimDelay, false);
	}
}

void AGPlayerCharacter::AttackDelay_Elapsed()
{
	// Blueprint has not been properly configured yet if this fails
	if (ensureAlways(ProjectileClass))
	{
		const FVector HandLocation = GetMesh()->GetSocketLocation(ProjectileSocketName);

		// We trace against the environment first to find whats under the player crosshair.
		// We use the hit location to adjust the projectile launch direction so it will hit what is under the crosshair rather than shoot straight forward from the player hands.

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;

		FCollisionShape Shape;
		Shape.SetSphere(SweepRadius);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		FVector TraceDirection = GetControlRotation().Vector();

		// Add sweep radius onto start to avoid the sphere clipping into floor/walls the camera is directly against.
		const FVector TraceStart = GetPawnViewLocation() + (TraceDirection * SweepRadius);
		// endpoint far into the look-at distance (not too far, still adjust somewhat towards crosshair on a miss)
		const FVector TraceEnd = TraceStart + (TraceDirection * SweepDistanceFallback);
		FVector AdjustedTraceEnd = TraceEnd;

		TArray<FHitResult> Hits;
		// Enemies are using Overlap response to Projectiles, we cant look for single Blocking hits and instead look for all overlaps and filter after
		if (GetWorld()->SweepMultiByChannel(Hits, TraceStart, TraceEnd, FQuat::Identity, COLLISION_PROJECTILE, Shape, Params))
		{
			// Overwrite trace end with impact point in world
			// First entry must exist and first entry will be first overlap or block
			// Could filter further, eg. ignoring friendly players between us and the enemy
			AdjustedTraceEnd = Hits[0].ImpactPoint;
		}

		// Removes debug code from shipping builds		
#if !UE_BUILD_SHIPPING
		//const float DrawDuration = 5.0f;
		// Start
		//DrawDebugPoint(GetWorld(), TraceStart, 8, FColor::Green, false, DrawDuration);
		// End - possibly adjusted based on hit
		//DrawDebugPoint(GetWorld(), AdjustedTraceEnd, 8, FColor::Green, false, DrawDuration);
		//DrawDebugLine(GetWorld(), TraceStart, AdjustedTraceEnd, FColor::Green, false, DrawDuration);
		// End - Original
		//DrawDebugPoint(GetWorld(), TraceEnd, 8, FColor::Red, false, DrawDuration);
		//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, DrawDuration);
#endif

		// find new direction/rotation from Hand pointing to impact point in world.

		FRotator ProjRotation = (AdjustedTraceEnd - HandLocation).Rotation();
		FTransform SpawnTM = FTransform(ProjRotation, HandLocation);

		//GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);

		// re-use a pooled actor instead of always spawning new Actors
		UUPActorPoolingSubsystem::AcquireFromPool(this, ProjectileClass, SpawnTM, SpawnParams);
	}
}

void AGPlayerCharacter::PlayAttackSound(USoundBase* InSound)
{
}

//void AGPlayerCharacter::LookMouse(const FInputActionValue& Value)
//{
//	const FVector2D InputValue = Value.Get<FVector2D>();
//
//	APawn::AddControllerYawInput(InputValue.X);
//	APawn::AddControllerPitchInput(InputValue.Y);
//}
