// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UPAICharacter.generated.h"

class UUPSignificanceComponent;
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

protected:
	virtual void PostInitializeComponents() override;

public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTargetActor() const;

protected:
	UFUNCTION(NetMulticast, Unreliable)
 	void MulticastPawnSeen();

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UUPAttributeComponent* OwningComp, float NewHealth, float Delta);

	UFUNCTION()
	void OnSignificanceChanged(ESignificanceValue Significance);

protected:
	UPROPERTY(Transient)
	TObjectPtr<UUPWorldUserWidget> ActiveHealthBar;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SpottedWidgetClass;

	/* Index must match the CustomPrimitiveData index used in the Overlay material */
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	int32 HitFlash_CustomPrimitiveIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UUPAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UUPActionComponent* ActionComponent;

	/* Handle fidelity for AI as they are off-screen or at far distances */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UUPSignificanceComponent> SigManComp;

	/* Key for AI Blackboard 'TargetActor' */
 	UPROPERTY(VisibleAnywhere, Category = "Effects")
 	FName TargetActorKey;
};
