// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GAICharacter.generated.h"

class UUPActionComponent;
struct FAttributeModification;
class UGDamagePopupWidget;

UCLASS()
class UEPRACTICE_API AGAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGAICharacter();

public:
	virtual void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	void Kill();

	void OnHealthAttributeChanged(float NewValue, const FAttributeModification& AttributeModification);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UUPActionComponent* ActionComponent;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UGDamagePopupWidget> DamagePopupWidgetClass;
};
