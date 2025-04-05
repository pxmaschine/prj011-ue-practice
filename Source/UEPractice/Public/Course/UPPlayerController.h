// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UPPlayerController.generated.h"


class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, APlayerState*, NewPlayerState);


UCLASS()
class UEPRACTICE_API AUPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AUPPlayerController();

protected:
	// Called when player controller is ready to begin playing, good moment to initialize things like UI which might be too early in BeginPlay
	// (esp. in multiplayer clients where not all data such as PlayerState may have been received yet)
	virtual void BeginPlayingState() override;

	virtual void OnRep_PlayerState() override;

	virtual void SetupInputComponent() override;

public:
	UFUNCTION(BlueprintPure, Category= "Input")
	bool IsUsingGamepad() const
	{
		return bIsUsingGamepad;
	}

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintBeginPlayingState();

	void AnyKeyInput(FKey PressedKey);

	void HandleToggleMenu();

protected:
	// Listen for incoming PlayerState (for clients this may be nullptr when initially joining the game,
	// afterwards player state will no longer change again as PlayerControllers maintain the same player state throughout the level)
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChanged OnPlayerStateReceived;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* InputAction_ToggleMenu;

private:
	/* Was any input recently using GamePad */
	bool bIsUsingGamepad;
};
