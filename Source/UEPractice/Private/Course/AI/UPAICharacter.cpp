// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/UPAICharacter.h"
#include "Course/UPAttributeComponent.h"
#include "Course/UPWorldUserWidget.h"
#include "Course/UPActionComponent.h"
#include "Course/UPSignificanceComponent.h"
#include "UEPractice/UEPractice.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"


AUPAICharacter::AUPAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	AttributeComponent = CreateDefaultSubobject<UUPAttributeComponent>(TEXT("AttributeComp"));
	ActionComponent = CreateDefaultSubobject<UUPActionComponent>(TEXT("ActionComp"));

	// Make sure to configure the distance values in Blueprint
	SigManComp = CreateDefaultSubobject<UUPSignificanceComponent>(TEXT("SigManComp"));

	// Enabled on mesh to react to incoming projectiles
	USkeletalMeshComponent* SkelMesh = GetMesh();
	SkelMesh->SetGenerateOverlapEvents(true);
	// Skip performing overlap queries on the Physics Asset after animation (17 queries in case of our MinionRangedBP)
	SkelMesh->bUpdateOverlapsOnAnimationFinalize = false;

	// Skip bones when not visible, may miss anim notifies etc. if animation is skipped so these options must be tested per use case
	SkelMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

	// Ensures we receive a controlled when spawned in the level by our gamemode
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	//TimeToHitParamName = "TimeToHit";
	HitFlash_CustomPrimitiveIndex = 0;
	TargetActorKey = "TargetActor";
}

void AUPAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AUPAICharacter::OnPawnSeen);
	AttributeComponent->OnHealthChanged.AddDynamic(this, &AUPAICharacter::OnHealthChanged);

	SigManComp->OnSignificanceChanged.AddDynamic(this, &AUPAICharacter::OnSignificanceChanged);
}

void AUPAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIC = GetController<AAIController>();
	AIC->GetBlackboardComponent()->SetValueAsObject(TargetActorKey, NewTarget);
}

AActor* AUPAICharacter::GetTargetActor() const
{
	AAIController* AIC = GetController<AAIController>();
	return Cast<AActor>(AIC->GetBlackboardComponent()->GetValueAsObject(TargetActorKey));
}

void AUPAICharacter::OnPawnSeen(APawn* Pawn)
{
	// Ignore if target already set
	if (GetTargetActor() != Pawn)
	{
		SetTargetActor(Pawn);

		MulticastPawnSeen();
	}

	//DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.0f, true);
}

void AUPAICharacter::MulticastPawnSeen_Implementation()
{
	UUPWorldUserWidget* NewWidget = CreateWidget<UUPWorldUserWidget>(GetWorld(), SpottedWidgetClass);

	// Can be nullptr if we didnt specify a class to use in Blueprint
	if (NewWidget)
	{
 		NewWidget->AttachedActor = this;
 		// Index of 10 (or anything higher than default of 0) places this on top of any other widget.
 		// May end up behind the minion health bar otherwise.
 		NewWidget->AddToViewport(10);
	}
}

void AUPAICharacter::OnHealthChanged(AActor* InstigatorActor, UUPAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)
	{
		if (InstigatorActor != this)
		{
			SetTargetActor(InstigatorActor);
		}

		// Create once, and skip on instant kill
		if (ActiveHealthBar == nullptr && NewHealth > 0.0)
		{
			ActiveHealthBar = CreateWidget<UUPWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if (ActiveHealthBar)
			{
				ActiveHealthBar->AttachedActor = this;
				ActiveHealthBar->AddToViewport();
			}
		}

		// Old way via MaterialInstanceDynamic, below implementation uses CustomPrimitiveData instead
		//GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

		// Replaces the above "old" method of requiring unique material instances for every mesh element on the player 
		GetMesh()->SetCustomPrimitiveDataFloat(HitFlash_CustomPrimitiveIndex, GetWorld()->TimeSeconds);


		// Died
		if (NewHealth <= 0.0f)
		{
			// Stop behavior tree
			const AAIController* AIC = GetController<AAIController>();
			AIC->GetBrainComponent()->StopLogic("Killed");

			// Enable ragdoll
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName(Collision::Ragdoll_ProfileName);

			// Disable colliders & movement
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCharacterMovement()->DisableMovement();

			// Set lifespan
			SetLifeSpan(10.0f);
		}
	}
}

void AUPAICharacter::OnSignificanceChanged(ESignificanceValue Significance)
{
	// @todo: this may not work perfectly with falling and similar movement modes. (We don't support this on the AI character anyway)
	// NavMesh based walking instead of using world geo
	if (Significance <= ESignificanceValue::Medium)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_NavWalking);
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	
	// Set as 'dormant' if actor is hidden, otherwise we continue ticking the entire character
	const bool bHiddenSignificance = Significance == ESignificanceValue::Hidden;
	SetActorTickEnabled(!bHiddenSignificance);
	GetCharacterMovement()->SetComponentTickEnabled(!bHiddenSignificance);


	EVisibilityBasedAnimTickOption AnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	switch (Significance)
	{
		// Example, force to always tick pose when really nearby. might need the pose even while offscreen
		case ESignificanceValue::Highest:
			AnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
			break;
		case ESignificanceValue::Medium:
			AnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
			break;
		case ESignificanceValue::Lowest:
		case ESignificanceValue::Hidden:
		case ESignificanceValue::Invalid:
			AnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	}
	
	GetMesh()->VisibilityBasedAnimTickOption = AnimTickOption;
}
