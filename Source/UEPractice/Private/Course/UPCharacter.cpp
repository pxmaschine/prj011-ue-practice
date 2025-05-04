// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPCharacter.h"
#include "Course/UPInteractionComponent.h"
#include "Course/UPActionComponent.h"
#include "Course/SharedGameplayTags.h"
#include "Course/UPPlayerController.h"
#include "Course/UPWorldUserWidget.h"
#include "Course/AI/UPAICharacter.h"
#include "UEPractice/UEPractice.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Course/UPGameModeBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"


// Sets default values
AUPCharacter::AUPCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);
	// We control the rotation of spring arm with pawn control rotation already, this disables a subtle side effect
	// where rotating our CapsuleComponent (eg. caused by bOrientRotationToMovement in Character Movement) will rotate our spring arm until it self corrects later in the update
	// This may cause unwanted effects when using CameraLocation during Tick as it may be slightly offset from our final camera position.
	SpringArmComp->SetUsingAbsoluteRotation(true);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<UUPInteractionComponent>(TEXT("InteractionComp"));

	ActionComponent = CreateDefaultSubobject<UUPActionComponent>(TEXT("ActionComp"));
	ActionComponent->SetDefaultAttributeSet(FUPSurvivorAttributeSet::StaticStruct());

	PerceptionStimuliComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliComp"));

	AttackSoundsComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AttackSoundsComp"));
	AttackSoundsComp->SetupAttachment(RootComponent);
	AttackSoundsComp->bAutoActivate = false;
	// Don't follow player unless actively playing a sound
	AttackSoundsComp->bAutoManageAttachment = true;

	UCharacterMovementComponent* CharMoveComp = GetCharacterMovement();
	CharMoveComp->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	// Attempt to make the jump feel less floaty
	CharMoveComp->JumpZVelocity = 550;
	CharMoveComp->GravityScale = 1.35f;
	CharMoveComp->BrakingDecelerationFalling = 200.f;

	// Enabled on mesh to react to incoming projectiles
	GetMesh()->SetGenerateOverlapEvents(true);

	// Skip performing overlap queries on the Physics Asset after animation (7 queries in case of our Gideon mesh)
	GetMesh()->bUpdateOverlapsOnAnimationFinalize = false;

	// Disable on capsule collision to avoid double-dipping and receiving 2 overlaps when entering trigger zones etc.
	// Once from the mesh, and 2nd time from capsule
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	//TimeToHitParamName = "TimeToHit";
	HitFlash_CustomPrimitiveIndex = 0;
}

void AUPCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ActionComponent->GetAttribute(SharedGameplayTags::Attribute_Health)->OnAttributeChanged.AddUObject(this, &ThisClass::OnHealthAttributeChanged);

	// Cheap trick to disable until we need it in the health event
	CachedOverlayMaxDistance = GetMesh()->OverlayMaterialMaxDrawDistance;
	GetMesh()->SetOverlayMaterialMaxDrawDistance(1);
}

void AUPCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(OverlayTimerHandle);
	GetWorldTimerManager().ClearAllTimersForObject(this);

	ActionComponent->GetAttribute(SharedGameplayTags::Attribute_Health)->OnAttributeChanged.RemoveAll(this);

	Super::EndPlay(EndPlayReason);
}

void AUPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FindCrosshairTarget();
}

void AUPCharacter::PlayAttackSound(USoundBase* InSound)
{
	// This may interrupt previously playing sounds, so you'd want to test for this
	AttackSoundsComp->SetSound(InSound);
	AttackSoundsComp->Play();
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

void AUPCharacter::LookMouse(const FInputActionValue& Value)
{
	FVector2D InputValue = Value.Get<FVector2D>();

	APawn::AddControllerYawInput(InputValue.X);
	APawn::AddControllerPitchInput(InputValue.Y);
}

void AUPCharacter::LookStick(const FInputActionValue& Value)
{
	FVector2D InputValue = Value.Get<FVector2D>();

	// Track negative as we'll lose this during the conversion
 	bool XNegative = InputValue.X < 0.f;
 	bool YNegative = InputValue.Y < 0.f;

	// Can further modify with 'sensitivity' settings
 	static const float LookYawRate = 100.0f;
 	static const float LookPitchRate = 50.0f;

	// non-linear to make aiming a little easier
 	InputValue = InputValue * InputValue;

	if (XNegative)
	{
 		InputValue.X *= -1.f;
 	}
 	if (YNegative)
 	{
 		InputValue.Y *= -1.f;
 	}

	// Aim assist
	// todo: may need to ease this out and/or change strength based on distance to target
	float RateMultiplier = 1.0f;
	if (bHasPawnTarget)
	{
		RateMultiplier = 0.5f;
	}

	AddControllerYawInput(InputValue.X * (LookYawRate * RateMultiplier) * GetWorld()->GetDeltaSeconds());
	AddControllerPitchInput(InputValue.Y * (LookPitchRate * RateMultiplier) * GetWorld()->GetDeltaSeconds());
}


void AUPCharacter::SprintStart()
{
	ActionComponent->StartActionByName(this, SharedGameplayTags::Action_Sprint);
}

void AUPCharacter::SprintStop()
{
	ActionComponent->StopActionByName(this, SharedGameplayTags::Action_Sprint);
}

void AUPCharacter::PrimaryAttack()
{
	ActionComponent->StartActionByName(this, SharedGameplayTags::Action_PrimaryAttack);
}

void AUPCharacter::SecondaryAttack()
{
	ActionComponent->StartActionByName(this, SharedGameplayTags::Action_Blackhole);
}

void AUPCharacter::Dash()
{
	ActionComponent->StartActionByName(this, SharedGameplayTags::Action_Dash);
}

void AUPCharacter::PrimaryInteract()
{
	InteractionComp->PrimaryInteract();
}

void AUPCharacter::OnHealthAttributeChanged(float NewValue, const FAttributeModification& AttributeModification)
{
	// Damaged
	if (AttributeModification.Magnitude < 0.0f)
	{
		// Materials, including the mesh "OverlayMaterial" can get their data via the component
		GetMesh()->SetCustomPrimitiveDataFloat(HitFlash_CustomPrimitiveIndex, GetWorld()->TimeSeconds);

		// Activate, we can skip rendering this at a distance
		GetMesh()->SetOverlayMaterialMaxDrawDistance(CachedOverlayMaxDistance);

		// After 1.0seconds we should be finished with the hitflash (re-use the handle to reset timer if we get hit again)
		GetWorldTimerManager().SetTimer(OverlayTimerHandle, this, &AUPCharacter::OnHitFlashFinshed, 1.0f, false);

		// Rage added equal to damage received (Abs to turn into positive rage number)
		const float RageDelta = FMath::Abs(AttributeModification.Magnitude);
		ActionComponent->ApplyAttributeChange(
			SharedGameplayTags::Attribute_Rage, 
			RageDelta, 
			this, 
			EAttributeModifyType::AddModifier);

		//UGameplayStatics::PlaySoundAtLocation(this, TakeDamageVOSound, GetActorLocation(), FRotator::ZeroRotator);
	}

	// Died
	if (NewValue <= 0.0f && AttributeModification.Magnitude < 0.0f)
	{
		//UGameplayStatics::PlaySoundAtLocation(this, DeathVOSound, GetActorLocation(), FRotator::ZeroRotator);

		PlayAnimMontage(DeathMontage);

		SetLifeSpan(5.0f);

		// Prevent bots from seeing us as a threat
		PerceptionStimuliComp->UnregisterFromPerceptionSystem();

		APlayerController* PC = GetController<AUPPlayerController>();
		if (PC && PC->IsLocalController())
		{
			//UGameplayStatics::PlaySound2D(this, DeathUISound);

			DisableInput(PC);
		}

		if (AUPGameModeBase* GM = GetWorld()->GetAuthGameMode<AUPGameModeBase>())
		{
			GM->OnActorKilled(this, AttributeModification.Instigator.Get());
		}
	}
}

void AUPCharacter::OnHitFlashFinshed()
{
	// Cheap trick to skip rendering this all the time unless we are actively hit flashing
	GetMesh()->SetOverlayMaterialMaxDrawDistance(1);
}

void AUPCharacter::FindCrosshairTarget()
{
	// Ignore if not using GamePad
	AUPPlayerController* PC = GetController<AUPPlayerController>();

	// Only use aim assist when currently controlled and using gamepad
	// Note: you *may* always want to line trace if using this result for other things like coloring crosshair or re-using this hit data for aim adjusting during projectile attacks
	if (PC == nullptr || !PC->IsUsingGamepad())
	{
		bHasPawnTarget = false;
		return;
	}

	FVector EyeLocation;
	FRotator EyeRotation;
	GetActorEyesViewPoint(EyeLocation, EyeRotation);

	const float AimAssistDistance = 5000.f;
	const FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * AimAssistDistance);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FCollisionShape Shape;
	Shape.SetSphere(50.f);

	// Called next frame when the trace has completed
	FTraceDelegate Delegate = FTraceDelegate::CreateUObject(this, &AUPCharacter::CrosshairTraceComplete);
	
	TraceHandle = GetWorld()->AsyncSweepByChannel(EAsyncTraceType::Single, EyeLocation, TraceEnd, FQuat::Identity, ECC_Pawn, Shape, Params, FCollisionResponseParams::DefaultResponseParam, &Delegate);
}

void AUPCharacter::CrosshairTraceComplete(const FTraceHandle& InTraceHandle, FTraceDatum& InTraceDatum)
{
	// at most expect one hit
	if (InTraceDatum.OutHits.IsValidIndex(0))
	{
		FHitResult Hit = InTraceDatum.OutHits[0];
		// Figure out if dealing with a Pawn, may want aim assist on other 'things', which requires a different check
		bHasPawnTarget = Hit.IsValidBlockingHit() && Hit.GetActor()->IsA(APawn::StaticClass());

		//UE_LOG(LogGame, Log, TEXT("has pawn target: %s"), bHasPawnTarget ? TEXT("true") : TEXT("false"));
	}
}

// Called to bind functionality to input
void AUPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
	InputComp->BindAction(Input_Move, ETriggerEvent::Triggered, this, &AUPCharacter::Move);
	InputComp->BindAction(Input_Jump, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	InputComp->BindAction(Input_Interact, ETriggerEvent::Triggered, this, &AUPCharacter::PrimaryInteract);

	// MKB
	InputComp->BindAction(Input_LookMouse, ETriggerEvent::Triggered, this, &AUPCharacter::LookMouse);
	// Gamepad
	InputComp->BindAction(Input_LookStick, ETriggerEvent::Triggered, this, &AUPCharacter::LookStick);

	// Sprint while key is held
	InputComp->BindAction(Input_Sprint, ETriggerEvent::Started, this, &AUPCharacter::SprintStart);
	InputComp->BindAction(Input_Sprint, ETriggerEvent::Completed, this, &AUPCharacter::SprintStop);

	// Abilities
	InputComp->BindAction(Input_PrimaryAttack, ETriggerEvent::Triggered, this, &AUPCharacter::PrimaryAttack);
	InputComp->BindAction(Input_SecondaryAttack, ETriggerEvent::Triggered, this, &AUPCharacter::SecondaryAttack);
	InputComp->BindAction(Input_Dash, ETriggerEvent::Triggered, this, &AUPCharacter::Dash);
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

FGenericTeamId AUPCharacter::GetGenericTeamId() const
{
	// We have no team switching support during gameplay
	return FGenericTeamId(TEAM_ID_PLAYERS);
}

void AUPCharacter::ClientOnSeenBy_Implementation(AUPAICharacter* SeenByPawn)
{
	// Can be nullptr if we do not specify a class to use in Blueprint
	if (UUPWorldUserWidget* NewWidget = CreateWidget<UUPWorldUserWidget>(GetWorld(), SpottedWidgetClass))
	{
		NewWidget->AttachedActor = SeenByPawn;
		UUPWorldUserWidget::AddToRootCanvasPanel(NewWidget);
	}
}
