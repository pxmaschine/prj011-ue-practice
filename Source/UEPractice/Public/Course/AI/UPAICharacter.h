// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UPAICharacter.generated.h"

class UUPActionComponent;
class UUPWorldUserWidget;
class UPawnSensingComponent;
class UPAttributeComponent;
class UUserWidget;

UCLASS()
class UEPRACTICE_API AUPAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUPAICharacter();

public:
	bool HasLowHealth() const;

protected:
	virtual void PostInitializeComponents() override;

	void SetTargetActor(AActor* NewTarget);

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UUPAttributeComponent* OwningComp, float NewHealth, float Delta);

protected:
	UUPWorldUserWidget* ActiveHealthBar;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	// UPawnSensingComponent is an older implementation of AI perception.
	// It has less features, but is also easier to use.
	// AI Perception is the modern API. It allows more customization.
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UUPAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UUPActionComponent* ActionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParamName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	float LowHealthThreshold;
};
