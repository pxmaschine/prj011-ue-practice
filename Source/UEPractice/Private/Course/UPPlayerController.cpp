// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPPlayerController.h"
#include "Course/UPCheatManager.h"
#include "Course/UPHUD.h"

#include "EnhancedInputComponent.h"

AUPPlayerController::AUPPlayerController()
{
	CheatClass = UUPCheatManager::StaticClass();
	bIsUsingGamepad = false;
}

void AUPPlayerController::BeginPlayingState()
{
	BlueprintBeginPlayingState();
}

void AUPPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	OnPlayerStateReceived.Broadcast(PlayerState);
}

void AUPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* InputComp = Cast<UEnhancedInputComponent>(InputComponent))
	{
		InputComp->BindAction(InputAction_ToggleMenu, ETriggerEvent::Started, this, &AUPPlayerController::HandleToggleMenu);
	}

	// Keeping as 'old' input for now until we figure out how to do this easily in Enhanced input
	InputComponent->BindAction("AnyKey", IE_Pressed, this, &AUPPlayerController::AnyKeyInput);
}

void AUPPlayerController::HandleToggleMenu()
{
	Cast<AUPHUD>(GetHUD())->TogglePauseMenu();
}


void AUPPlayerController::AnyKeyInput(FKey PressedKey)
{
	bIsUsingGamepad = PressedKey.IsGamepadKey();
}
