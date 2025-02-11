// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UPAICharacter.generated.h"

class UPawnSensingComponent;

UCLASS()
class UEPRACTICE_API AUPAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUPAICharacter();

protected:
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

protected:
	// UPawnSensingComponent is an older implementation of AI perception.
	// It has less features, but is also easier to use.
	// AI Perception is the modern API. It allows more customization.
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp;
};
