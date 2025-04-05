// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPCheatManager.h"
#include "Course/UPAttributeComponent.h"
#include "Course/AI/UPAICharacter.h"
#include "Course/UPSaveGameSettings.h"

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

void UUPCheatManager::HealSelf(float Amount)
{
	const APlayerController* MyPC = GetOuterAPlayerController();

	if (APawn* MyPawn = MyPC->GetPawn())
	{
		UUPAttributeComponent* AttributeComp = MyPawn->FindComponentByClass<UUPAttributeComponent>();
		AttributeComp->ApplyHealthChange(MyPawn, Amount);
	}
}

void UUPCheatManager::KillAll()
{
	for (const AUPAICharacter* Bot : TActorRange<AUPAICharacter>(GetWorld()))
	{
		UUPAttributeComponent* AttributeComp = UUPAttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(GetOuterAPlayerController()->GetPawn());
		}
	}
}

void UUPCheatManager::DeleteSaveGame()
{
	const UUPSaveGameSettings* SGSettings = GetDefault<UUPSaveGameSettings>();
	UGameplayStatics::DeleteGameInSlot(SGSettings->SaveSlotName, 0);
}
