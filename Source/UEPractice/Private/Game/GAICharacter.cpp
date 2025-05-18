// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GAICharacter.h"
#include <Game/GGameModeBase.h>
#include <Game/GAttributeSet.h>
#include <Game/GDamagePopupWidget.h>
#include "Course/UPActionComponent.h"
#include "Course/SharedGameplayTags.h"
#include <UEPractice/UEPractice.h>

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"

// Sets default values
AGAICharacter::AGAICharacter()
{
	ActionComponent = CreateDefaultSubobject<UUPActionComponent>(TEXT("ActionComp"));
	// Set some defaults, ideally we handle this through some data asset instead
	ActionComponent->SetDefaultAttributeSet(FGEnemyAttributeSet::StaticStruct());

	// Enabled on mesh to react to incoming projectiles
	USkeletalMeshComponent* SkelMesh = GetMesh();
	SkelMesh->SetGenerateOverlapEvents(true);
	// Skip performing overlap queries on the Physics Asset after animation (17 queries in case of our MinionRangedBP)
	SkelMesh->bUpdateOverlapsOnAnimationFinalize = false;

	// Skip bones when not visible, may miss anim notifies etc. if animation is skipped so these options must be tested per use case
	SkelMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;

	// Ensures we receive a controlled when spawned in the level by our gamemode
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCharacterMovement()->MaxWalkSpeed = 150.0f;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	bUseControllerRotationYaw = false;
}

void AGAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// The "simplest" syntax compared to the other convoluted attempts
	FUPAttribute* FoundAttribute = ActionComponent->GetAttribute(SharedGameplayTags::Attribute_Health);
	FoundAttribute->OnAttributeChanged.AddUObject(this, &ThisClass::OnHealthAttributeChanged);
}

// Called when the game starts or when spawned
void AGAICharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AGAICharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearAllTimersForObject(this);

	FUPAttribute* FoundAttribute = ActionComponent->GetAttribute(SharedGameplayTags::Attribute_Health);
	FoundAttribute->OnAttributeChanged.RemoveAll(this);

	Super::EndPlay(EndPlayReason);
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

void AGAICharacter::OnHealthAttributeChanged(float NewValue, const FAttributeModification& AttributeModification)
{
	const float Delta = AttributeModification.Magnitude;
	//AActor* InstigatorActor = AttributeModification.Instigator.Get();
	
	if (Delta < 0.0f)
	{
		UGDamagePopupWidget* DamagePopupWidget = CreateWidget<UGDamagePopupWidget>(GetWorld(), DamagePopupWidgetClass);
		if (DamagePopupWidget)
		{
			DamagePopupWidget->AttachedActor = this;
			FString DamageText = FString::Printf(TEXT("%.0f"), FMath::Abs(Delta));
			DamagePopupWidget->SetDamageText(DamageText);
			UUPWorldUserWidget::AddToRootCanvasPanel(DamagePopupWidget);
		}

		//// Create once, and skip on instant kill
		//if (ActiveHealthBar == nullptr && NewValue > 0.0)
		//{
		//	ActiveHealthBar = CreateWidget<UUPWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
		//	if (ActiveHealthBar)
		//	{
		//		ActiveHealthBar->AttachedActor = this;
		//		UUPWorldUserWidget::AddToRootCanvasPanel(ActiveHealthBar);
		//	}
		//}

		//// Replaces the above "old" method of requiring unique material instances for every mesh element on the player 
		//GetMesh()->SetCustomPrimitiveDataFloat(HitFlash_CustomPrimitiveIndex, GetWorld()->TimeSeconds);

		//// We can skip rendering this at a distance
		//GetMesh()->SetOverlayMaterialMaxDrawDistance(CachedOverlayMaxDistance);

		//// After 1.0seconds we should be finished with the hitflash (re-use the handle to reset timer if we get hit again)
		//GetWorldTimerManager().SetTimer(OverlayTimerHandle, this, &AUPAICharacter::OnHitFlashFinshed, 1.0f, false);

		// Died
		if (NewValue <= 0.0f)
		{
			//// Stop behavior tree
			//if (HasAuthority())
			//{
			//	const AAIController* AIC = GetController<AAIController>();
			//	AIC->GetBrainComponent()->StopLogic("Killed");
			//	
			//	// Clears active actions, and (de)buffs.
			//	ActionComponent->StopAllActions();
			//}

			Kill();

			//// Enable ragdoll
			//GetMesh()->SetAllBodiesSimulatePhysics(true);
			//GetMesh()->SetCollisionProfileName(Collision::Ragdoll_ProfileName);

			//// Disable colliders & movement
			//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//GetCharacterMovement()->DisableMovement();

			//// Set lifespan
			//SetLifeSpan(10.0f);

			//if (AUPGameModeBase* GM = GetWorld()->GetAuthGameMode<AUPGameModeBase>())
			//{
			//	GM->OnActorKilled(GetOwner(), AttributeModification.Instigator.Get());
			//}
		}
		else
		{
			//UAISense_Damage::ReportDamageEvent(this, this, InstigatorActor, FMath::Abs(Delta),
			//	InstigatorActor->GetActorLocation(), GetActorLocation());
		}
	}
}
