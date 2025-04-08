// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPCheatManager.h"
#include "Course/AI/UPAICharacter.h"
#include "Course/UPSaveGameSettings.h"
#include "Course/SharedGameplayTags.h"
#include "Course/UPAttributeSet.h"
#include "Course/UPActionComponent.h"
#include "Course/UPGameplayFunctionLibrary.h"

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

void UUPCheatManager::HealSelf(float Amount)
{
	const APlayerController* MyPC = GetOuterAPlayerController();

	if (APawn* MyPawn = MyPC->GetPawn())
	{
		UUPActionComponent* ActionComp = UUPActionComponent::GetActionComponent(MyPawn);
		ActionComp->ApplyAttributeChange(SharedGameplayTags::Attribute_Health, Amount, MyPawn, EAttributeModifyType::AddModifier);
	}
}

void UUPCheatManager::KillAll()
{
	for (AUPAICharacter* Bot : TActorRange<AUPAICharacter>(GetWorld()))
	{
		UUPGameplayFunctionLibrary::KillActor(Bot);
	}
}

void UUPCheatManager::DeleteSaveGame()
{
	const UUPSaveGameSettings* SGSettings = GetDefault<UUPSaveGameSettings>();
	UGameplayStatics::DeleteGameInSlot(SGSettings->SaveSlotName, 0);
}
