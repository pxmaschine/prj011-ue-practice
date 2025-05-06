// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GAICharacter.generated.h"

UCLASS()
class UEPRACTICE_API AGAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
