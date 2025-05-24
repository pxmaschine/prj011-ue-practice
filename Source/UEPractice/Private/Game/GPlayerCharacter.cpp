// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GPlayerCharacter.h"
#include <Game/GGameplayFunctionLibrary.h>
#include <Game/GAttributeSet.h>
#include <Course/UPActorPoolingSubsystem.h>
#include "Course/UPActionComponent.h"
#include "Course/UPWorldUserWidget.h"
#include "Course/SharedGameplayTags.h"
#include <UEPractice/UEPractice.h>

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include <NiagaraFunctionLibrary.h>
#include "Components/AudioComponent.h"

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

	AttackSoundsComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AttackSoundsComp"));
	AttackSoundsComp->SetupAttachment(RootComponent);
	AttackSoundsComp->bAutoActivate = false;
	// Don't follow player unless actively playing a sound
	AttackSoundsComp->bAutoManageAttachment = true;

	ActionComponent = CreateDefaultSubobject<UUPActionComponent>(TEXT("ActionComp"));
	ActionComponent->SetDefaultAttributeSet(FGPlayerAttributeSet::StaticStruct());

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

	PrimaryAttackDelay = 1.2f;
}

// Called when the game starts or when spawned
void AGPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (ActiveHealthBar == nullptr)
	{
		ActiveHealthBar = CreateWidget<UUPWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
		if (ActiveHealthBar)
		{
			ActiveHealthBar->AttachedActor = this;
			UUPWorldUserWidget::AddToRootCanvasPanel(ActiveHealthBar);
		}
	}

	// Only on server
	if (HasAuthority())
	{
		FTimerHandle TimerHandle_AttackDelay;
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &AGPlayerCharacter::PrimaryAttack);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, PrimaryAttackDelay, true);
	}
}

// Called every frame
void AGPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TargetActor = UGGameplayFunctionLibrary::GetClosestEnemy(this);
	if (TargetActor)
	{
		// Get direction to target
		FVector ToTarget = TargetActor->GetActorLocation() - GetActorLocation();
		ToTarget.Z = 0.0f;

		// Smoothly rotate towards the target
		FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), ToTarget.Rotation(), DeltaTime, RotationSpeed);

		//// Remove pitch and roll to keep the character upright
		//NewRotation.Pitch = 0.0f;
		//NewRotation.Roll = 0.0f;

		// Apply new rotation
		Controller->SetControlRotation(NewRotation);
	}

	//// -- Rotation Visualization -- //
	//constexpr float DrawScale = 100.0f;
	//constexpr float Thickness = 5.0f;

	//FVector LineStart = GetActorLocation();
	//// Offset to the right of pawn
	//LineStart += GetActorRightVector() * 100.0f;
	//// Set line end in direction of the actor's forward
	//FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);
	//// Draw Actor's Direction
	//DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0, Thickness);

	//FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);
	//// Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character)
	//DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.0f, 0, Thickness);
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

	//InputComp->BindAction(Input_PrimaryAttack, ETriggerEvent::Triggered, this, &AGPlayerCharacter::PrimaryAttack);

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

        FVector CameraForward = CameraComp->GetForwardVector();
        FVector CameraRight = CameraComp->GetRightVector();

        CameraForward.Z = 0.0f;
        CameraRight.Z = 0.0f;
        CameraForward.Normalize();
        CameraRight.Normalize();

        AddMovementInput(CameraForward, InputValue.Y);
        AddMovementInput(CameraRight, InputValue.X);
	}
}

void AGPlayerCharacter::PrimaryAttack()
{
	ActionComponent->StartActionByName(this, SharedGameplayTags::Action_PrimaryAttack);
}


void AGPlayerCharacter::PlayAttackSound(USoundBase* InSound)
{
	// This may interrupt previously playing sounds, so you'd want to test for this
	AttackSoundsComp->SetSound(InSound);
	AttackSoundsComp->Play();
}

//void AGPlayerCharacter::LookMouse(const FInputActionValue& Value)
//{
//	const FVector2D InputValue = Value.Get<FVector2D>();
//
//	APawn::AddControllerYawInput(InputValue.X);
//	APawn::AddControllerPitchInput(InputValue.Y);
//}
