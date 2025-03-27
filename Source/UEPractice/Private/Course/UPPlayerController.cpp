// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPPlayerController.h"

#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

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

	// Keeping as 'old' input for now until we figure out how to do this easily in Enhanced input
	InputComponent->BindAction("AnyKey", IE_Pressed, this, &AUPPlayerController::AnyKeyInput);
}

void AUPPlayerController::TogglePauseMenu()
{
	if (PauseMenuInstance && PauseMenuInstance->IsInViewport())
	{
		PauseMenuInstance->RemoveFromParent();
		PauseMenuInstance = nullptr;

		SetShowMouseCursor(false);
		SetInputMode(FInputModeGameOnly());

		// Single-player only
		if (GetWorld()->IsNetMode(NM_Standalone))
		{
			UGameplayStatics::SetGamePaused(this, false);
		}

		return;
	}

	PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);

	if (PauseMenuInstance)
	{
		PauseMenuInstance->AddToViewport(100);

		SetShowMouseCursor(true);
		SetInputMode(FInputModeUIOnly());

		// Single-player only
		if (GetWorld()->IsNetMode(NM_Standalone))
		{
			UGameplayStatics::SetGamePaused(this, false);
		}
	}
}

void AUPPlayerController::AnyKeyInput(FKey PressedKey)
{
	bIsUsingGamepad = PressedKey.IsGamepadKey();
}
