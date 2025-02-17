// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Course/UPAttributeComponent.h"
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

public:
	bool HasLowHealth() const { return AttributeComponent->GetCurrentHealth() < LowHealthThreshold; }

protected:
	virtual void PostInitializeComponents() override;

	void SetTargetActor(AActor* NewTarget);

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UUPAttributeComponent* OwningComp, float NewHealth, float Delta);

protected:
	// UPawnSensingComponent is an older implementation of AI perception.
	// It has less features, but is also easier to use.
	// AI Perception is the modern API. It allows more customization.
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UUPAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParamName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	float LowHealthThreshold;
};
