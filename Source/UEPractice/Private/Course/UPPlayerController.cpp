// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPPlayerController.h"

#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"

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
		InputComp->BindAction(InputAction_ToggleMenu, ETriggerEvent::Started, this, &AUPPlayerController::TogglePauseMenu);
	}
}

void AUPPlayerController::TogglePauseMenu()
{
	if (PauseMenuInstance && PauseMenuInstance->IsInViewport())
	{
		PauseMenuInstance->RemoveFromParent();
		PauseMenuInstance = nullptr;

		SetShowMouseCursor(false);
		SetInputMode(FInputModeGameOnly());

		return;
	}

	PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);

	if (PauseMenuInstance)
	{
		PauseMenuInstance->AddToViewport(100);

		SetShowMouseCursor(true);
		SetInputMode(FInputModeUIOnly());
	}
}
