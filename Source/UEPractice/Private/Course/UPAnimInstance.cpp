// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPAnimInstance.h"
#include "Course/UPActionComponent.h"

#include "GameplayTagContainer.h"

void UUPAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (const AActor* OwningActor = GetOwningActor())
	{
		ActionComp = Cast<UUPActionComponent>(OwningActor->GetComponentByClass(UUPActionComponent::StaticClass()));
	}
}

void UUPAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	static FGameplayTag StunnedTag = FGameplayTag::RequestGameplayTag("Status.Stunned");

	if (ActionComp)
	{
		bIsStunned = ActionComp->ActiveGameplayTags.HasTag(StunnedTag);
	}
}
