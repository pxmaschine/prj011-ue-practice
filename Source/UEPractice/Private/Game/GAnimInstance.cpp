// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GAnimInstance.h"
#include "Course/UPActionComponent.h"
#include "Course/SharedGameplayTags.h"

#include "GameplayTagContainer.h"

void UGAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	const AActor* OwningActor = GetOwningActor();
	check(OwningActor);

	ActionComp = OwningActor->FindComponentByClass<UUPActionComponent>();
}

void UGAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}
