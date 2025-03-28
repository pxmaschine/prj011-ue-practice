// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPAnimInstance.h"
#include "Course/UPActionComponent.h"
#include "Course/SharedGameplayTags.h"

#include "GameplayTagContainer.h"

void UUPAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	const AActor* OwningActor = GetOwningActor();
	check(OwningActor);

	ActionComp = OwningActor->FindComponentByClass<UUPActionComponent>();
}

void UUPAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (ActionComp)
	{
		bIsStunned = ActionComp->ActiveGameplayTags.HasTag(SharedGameplayTags::Action_Stunned);
	}
}
