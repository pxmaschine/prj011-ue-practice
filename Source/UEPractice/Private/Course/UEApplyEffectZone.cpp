// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UEApplyEffectZone.h"
#include "Course/UPActionComponent.h"


AUEApplyEffectZone::AUEApplyEffectZone()
{
	ActionComponent = CreateDefaultSubobject<UUPActionComponent>(TEXT("ActionComp"));
}
