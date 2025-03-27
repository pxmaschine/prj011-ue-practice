// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPCharacter.h"
#include "Course/UPAttributeComponent.h"
#include "Course/UPInteractionComponent.h"
#include "Course/UPActionComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AUPCharacter::AUPCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);
	// We control the rotation of spring arm with pawn control rotation already, this disables a subtle side effect
	// where rotating our CapsuleComponent (eg. caused by bOrientRotationToMovement in Character Movement) will rotate our spring arm until it self corrects later in the update
	// This may cause unwanted effects when using CameraLocation during Tick as it may be slightly offset from our final camera position.
	SpringArmComp->SetUsingAbsoluteRotation(true);

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<UUPInteractionComponent>("InteractionComp");

	AttributeComponent = CreateDefaultSubobject<UUPAttributeComponent>("AttributeComp");

	ActionComponent = CreateDefaultSubobject<UUPActionComponent>("ActionComp");

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	// Enabled on mesh to react to incoming projectiles
	GetMesh()->SetGenerateOverlapEvents(true);

	// Skip performing overlap queries on the Physics Asset after animation (7 queries in case of our Gideon mesh)
	GetMesh()->bUpdateOverlapsOnAnimationFinalize = false;

	// Disable on capsule collision to avoid double-dipping and receiving 2 overlaps when entering trigger zones etc.
	// Once from the mesh, and 2nd time from capsule
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	TimeToHitParamName = "TimeToHit";
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

void AUPCharacter::SprintStart()
{
	ActionComponent->StartActionByName(this, "Sprint");
}

void AUPCharacter::SprintStop()
{
	ActionComponent->StopActionByName(this, "Sprint");
}

void AUPCharacter::PrimaryAttack()
{
	ActionComponent->StartActionByName(this, "PrimaryAttack");
}

void AUPCharacter::BlackholeAttack()
{
	ActionComponent->StartActionByName(this, "BlackholeAttack");
}

void AUPCharacter::Dash()
{
	ActionComponent->StartActionByName(this, "DashAttack");
}

void AUPCharacter::PrimaryInteract()
{
	InteractionComp->PrimaryInteract();
}

void AUPCharacter::OnHealthChanged(AActor* InstigatorActor, UUPAttributeComponent* OwningComp, float NewValue,
	float Delta)
{
	// Damaged
	if (Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

		// Rage added equal to damage received (Abs to turn into positive rage number)
		float RageDelta = FMath::Abs(Delta);
		OwningComp->ApplyRageChange(InstigatorActor, RageDelta);
	}

	// Died
	if (NewValue <= 0.0f && Delta < 0.0f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);

		SetLifeSpan(5.0f);
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
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AUPCharacter::SprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AUPCharacter::SprintStop);
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

FVector AUPCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

void AUPCharacter::HealSelf(float Amount /* = 100.0f */)
{
	AttributeComponent->ApplyHealthChange(this, Amount);
}

