// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/UPAICharacter.h"

// Sets default values
AUPAICharacter::AUPAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AUPAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUPAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
