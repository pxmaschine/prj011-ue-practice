// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GAction_EnemyMeleeAttack.h"
#include "Game/GAnimInstance.h"
#include "Course/UPGameplayFunctionLibrary.h"
#include "Course/SharedGameplayTags.h"
#include "Course/UPActionComponent.h"
#include "UEPractice/UEPractice.h"

#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include <Game/GPlayerCharacter.h>


UGAction_EnemyMeleeAttack::UGAction_EnemyMeleeAttack()
{
	ActivationTag = SharedGameplayTags::Action_Melee;
}

void UGAction_EnemyMeleeAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	bDamageApplied = false;

	check(IsInGameThread());
	
	ACharacter* MyPawn = CastChecked<ACharacter>(Instigator);
	UGAnimInstance* AnimInst = Cast<UGAnimInstance>(MyPawn->GetMesh()->GetAnimInstance());
	OnOverlapHandle = AnimInst->OnMeleeOverlap.AddUObject(this, &ThisClass::OnMeleeOverlaps);

	const float Duration = MeleeMontage->GetSectionLength(0);
	MyPawn->PlayAnimMontage(MeleeMontage);

	// Stop once the animation has finished
	FTimerHandle AnimCompleteHandle;
	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &ThisClass::StopAction, Instigator);
	GetWorld()->GetTimerManager().SetTimer(AnimCompleteHandle, Delegate, Duration, false);
}

void UGAction_EnemyMeleeAttack::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

	ACharacter* MyPawn = CastChecked<ACharacter>(Instigator);
	UGAnimInstance* AnimInst = Cast<UGAnimInstance>(MyPawn->GetMesh()->GetAnimInstance());
	AnimInst->OnMeleeOverlap.Remove(OnOverlapHandle);
}

//void UGAction_EnemyMeleeAttack::BeginDestroy()
//{
//	UUPAction::BeginDestroy();
//
//	if (AnimCompleteHandle.IsValid())
//	{
//		GetWorld()->GetTimerManager().ClearTimer(AnimCompleteHandle);
//	}
//}

void UGAction_EnemyMeleeAttack::OnMeleeOverlaps(const TArray<FOverlapResult>& Overlaps)
{
	if (bDamageApplied)
	{
		// Skip
		// todo: alternatively, we could still allow one damage per actor, so it can melee-hit multiple different actors
		// across multiple "triggers" of a melee animation
		return;
	}
	
	// Find "best" overlap and deal damage to it.
	AGPlayerCharacter* Player = nullptr;
	for (const FOverlapResult& Overlap : Overlaps)
	{
		Player = Cast<AGPlayerCharacter>(Overlap.GetActor());
		if (Player)
		{
			break;
		}
	}

	if (Player == nullptr)
	{
		return;
	}

	UUPGameplayFunctionLibrary::ApplyDamage(GetOwningComponent()->GetOwner(), Player, 100.0f);

	// Only allow damage once
	bDamageApplied = true;
}
