// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UPHUD.generated.h"

/**
 * 
 */
UCLASS()
class UEPRACTICE_API AUPHUD : public AHUD
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu();
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> PauseMenuInstance;
};
