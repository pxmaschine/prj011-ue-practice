// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/UPAICharacter.h"
#include "Course/UPAttributeComponent.h"
#include "Course/UPWorldUserWidget.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"


AUPAICharacter::AUPAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	AttributeComponent = CreateDefaultSubobject<UUPAttributeComponent>("AttributeComp");

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	LowHealthThreshold = 30.0f;
	TimeToHitParamName = "TimeToHit";
}

bool AUPAICharacter::HasLowHealth() const
{
	return AttributeComponent->GetCurrentHealth() < LowHealthThreshold;
}

void AUPAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AUPAICharacter::OnPawnSeen);
	AttributeComponent->OnHealthChanged.AddDynamic(this, &AUPAICharacter::OnHealthChanged);
}

void AUPAICharacter::SetTargetActor(AActor* NewTarget)
{
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->GetBlackboardComponent()->SetValueAsObject("TargetActor", NewTarget);
	}
}

void AUPAICharacter::OnPawnSeen(APawn* Pawn)
{
	SetTargetActor(Pawn);

	DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.0f, true);
}

void AUPAICharacter::OnHealthChanged(AActor* InstigatorActor, UUPAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)
	{
		if (InstigatorActor != this)
		{
			SetTargetActor(InstigatorActor);
		}

		if (ActiveHealthBar == nullptr)
		{
			ActiveHealthBar = CreateWidget<UUPWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if (ActiveHealthBar)
			{
				ActiveHealthBar->AttachedActor = this;
				ActiveHealthBar->AddToViewport();
			}
		}

		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

		// Died
		if (NewHealth <= 0.0f)
		{
			// Stop behavior tree
			AAIController* AIC = Cast<AAIController>(GetController());
			if (AIC)
			{
				AIC->GetBrainComponent()->StopLogic("Killed");
			}

			// Enable ragdoll
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName("Ragdoll");

			// Disable colliders & movement
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCharacterMovement()->DisableMovement();

			// Set lifespan
			SetLifeSpan(10.0f);
		}
	}
}
