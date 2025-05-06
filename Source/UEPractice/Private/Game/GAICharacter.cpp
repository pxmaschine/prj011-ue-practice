// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GAICharacter.h"
#include <Game/GGameModeBase.h>
#include <UEPractice/UEPractice.h>

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AGAICharacter::AGAICharacter()
{
	// Enabled on mesh to react to incoming projectiles
	USkeletalMeshComponent* SkelMesh = GetMesh();
	SkelMesh->SetGenerateOverlapEvents(true);
	// Skip performing overlap queries on the Physics Asset after animation (17 queries in case of our MinionRangedBP)
	SkelMesh->bUpdateOverlapsOnAnimationFinalize = false;

	// Skip bones when not visible, may miss anim notifies etc. if animation is skipped so these options must be tested per use case
	SkelMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

	// Ensures we receive a controlled when spawned in the level by our gamemode
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AGAICharacter::Kill()
{
	// Enable ragdoll
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(Collision::Ragdoll_ProfileName);

	// Disable colliders & movement
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	SetLifeSpan(10.0f);

	if (AGGameModeBase* GM = GetWorld()->GetAuthGameMode<AGGameModeBase>())
	{
		GM->OnEnemyKilled(this);
	}
}

// Called when the game starts or when spawned
void AGAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
