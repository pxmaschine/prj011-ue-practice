// Fill out your copyright notice in the Description page of Project Settings.


#include "UPCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UPAttributeComponent.h"
#include "UPInteractionComponent.h"
#include "UPProjectileBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
AUPCharacter::AUPCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<UUPInteractionComponent>("InteractionComp");

	AttributeComponent = CreateDefaultSubobject<UUPAttributeComponent>("AttributeComp");

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	AttackAnimDelay = 0.2f;
	TimeToHitParamName = "TimeToHit";
	HandSocketName = "Muzzle_01";
}

void AUPCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComponent->OnHealthChanged.AddDynamic(this, &AUPCharacter::OnHealthChanged);
}

// Called when the game starts or when spawned
void AUPCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AUPCharacter::Move(const FInputActionValue& Value)
{
	FVector2D InputValue = Value.Get<FVector2D>();

	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	AddMovementInput(ControlRot.Vector(), InputValue.Y);
	AddMovementInput(FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y), InputValue.X);
}

void AUPCharacter::Look(const FInputActionValue& Value)
{
	FVector2D InputValue = Value.Get<FVector2D>();

	APawn::AddControllerYawInput(InputValue.X);
	APawn::AddControllerPitchInput(InputValue.Y);
}

void AUPCharacter::PrimaryAttack()
{
	StartAttackEffects();

	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &AUPCharacter::PrimaryAttack_TimeElapsed, AttackAnimDelay);
}

void AUPCharacter::PrimaryAttack_TimeElapsed()
{
	SpawnProjectile(MagicProjectileClass);
}

void AUPCharacter::BlackholeAttack()
{
	StartAttackEffects();

	GetWorldTimerManager().SetTimer(TimerHandle_BlackholeAttack, this, &AUPCharacter::BlackholeAttack_TimeElapsed, AttackAnimDelay);
}

void AUPCharacter::BlackholeAttack_TimeElapsed()
{
	SpawnProjectile(BlackholeProjectileClass);
}

void AUPCharacter::Dash()
{
	StartAttackEffects();

	GetWorldTimerManager().SetTimer(TimerHandle_Dash, this, &AUPCharacter::Dash_TimeElapsed, AttackAnimDelay);
}

void AUPCharacter::Dash_TimeElapsed()
{
	SpawnProjectile(DashProjectileClass);
}

void AUPCharacter::StartAttackEffects()
{
	PlayAnimMontage(AttackAnim);

	UGameplayStatics::SpawnEmitterAttached(CastVFX, GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
}

void AUPCharacter::SpawnProjectile(TSubclassOf<AUPProjectileBase> ProjectileClass)
{
	if (!ensureAlways(ProjectileClass))
	{
		return;
	}

	FVector EyeLocation = CameraComp->GetComponentLocation();
	FRotator EyeRotation = CameraComp->GetComponentRotation();
	const FVector End = EyeLocation + (EyeRotation.Vector() * 10000.0);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FHitResult Hit;
	bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams, Params);

	FVector ImpactLocation = bBlockingHit ? Hit.ImpactPoint : Hit.TraceEnd;
	USkeletalMeshComponent* MeshComp = GetMesh();
	const FTransform Hand = MeshComp->GetSocketTransform("Muzzle_01");
	const FVector HandLocation = Hand.GetLocation();

	FRotator ProjectileRotation = FRotationMatrix::MakeFromX(ImpactLocation - HandLocation).Rotator();

	FTransform SpawnTM = FTransform(ProjectileRotation, HandLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;

	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
}

void AUPCharacter::PrimaryInteract()
{
	InteractionComp->PrimaryInteract();
}

void AUPCharacter::OnHealthChanged(AActor* InstigatorActor, UUPAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if (Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);

		if (NewHealth <= 0.0f)
		{
			APlayerController* PC = Cast<APlayerController>(GetController());
			DisableInput(PC);
		}
	}
}

// Called every frame
void AUPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//const float DrawScale = 100.0f;
	//const float Thickness = 5.0f;

	//FVector LineStart = GetActorLocation();
	//// Offset to the right of pawn
	//LineStart += GetActorRightVector() * 100.0f;
	//// Set line end in direction of the actor's forward
	//FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);
	//// Draw Actor's Direction
	//DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0, Thickness);

	//FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);
	//// Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character
	//DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.0f, 0, Thickness);
}

// Called to bind functionality to input
void AUPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUPCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUPCharacter::Look);
		EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Triggered, this, &AUPCharacter::PrimaryAttack);
		EnhancedInputComponent->BindAction(UltimateAttackAction, ETriggerEvent::Triggered, this, &AUPCharacter::BlackholeAttack);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AUPCharacter::Dash);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AUPCharacter::PrimaryInteract);

	}
}

void AUPCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	if (CameraComp)
	{
		OutLocation = CameraComp->GetComponentLocation();
		OutRotation = CameraComp->GetComponentRotation();
	}
	else
	{
		Super::GetActorEyesViewPoint(OutLocation, OutRotation);
	}
}

