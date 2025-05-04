// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/UPAction_MinionMeleeAttack.h"
#include "Course/SharedGameplayTags.h"
#include "Course/UPActionComponent.h"
#include "Course/UPAnimInstance.h"
#include "Course/UPGameplayFunctionLibrary.h"

#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"


UUPAction_MinionMeleeAttack::UUPAction_MinionMeleeAttack()
{
	ActivationTag = SharedGameplayTags::Action_Melee;
}

void UUPAction_MinionMeleeAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	bDamageApplied = false;

	check(IsInGameThread());
	
	ACharacter* MyPawn = CastChecked<ACharacter>(Instigator);
	UUPAnimInstance* AnimInst = Cast<UUPAnimInstance>(MyPawn->GetMesh()->GetAnimInstance());
	OnOverlapHandle = AnimInst->OnMeleeOverlap.AddUObject(this, &ThisClass::OnMeleeOverlaps);

	const float Duration = MyPawn->PlayAnimMontage(MeleeMontage);

	// Stop once the animation has finished
	FTimerHandle AnimCompleteHandle;
	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &ThisClass::StopAction, Instigator);
	GetWorld()->GetTimerManager().SetTimer(AnimCompleteHandle, Delegate, Duration,false);
}

void UUPAction_MinionMeleeAttack::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

	ACharacter* MyPawn = CastChecked<ACharacter>(Instigator);
	UUPAnimInstance* AnimInst = Cast<UUPAnimInstance>(MyPawn->GetMesh()->GetAnimInstance());
	AnimInst->OnMeleeOverlap.Remove(OnOverlapHandle);
}

void UUPAction_MinionMeleeAttack::OnMeleeOverlaps(const TArray<FOverlapResult>& Overlaps)
{
	if (bDamageApplied)
	{
		// Skip
		// todo: alternatively, we could still allow one damage per actor, so it can melee-hit multiple different actors
		// across multiple "triggers" of a melee animation
		return;
	}
	
	// Find "best" overlap and deal damage to it.
	AActor* BestOverlap = nullptr;
	for (const FOverlapResult& Overlap : Overlaps)
	{
		// todo: check if damagable, if enemy, etc.
		if (Overlap.GetActor()->CanBeDamaged())
		{
			BestOverlap = Overlap.GetActor();
			break;
		}
	}

	// @todo: use minion attackDamageAttribute * meleeDmgCoefficient
	constexpr float DamageAmount = 20.f;

	UUPGameplayFunctionLibrary::ApplyDamage(GetOwningComponent()->GetOwner(), BestOverlap, DamageAmount);

	// Only allow damage once
	bDamageApplied = true;
}
