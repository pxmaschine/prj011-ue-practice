// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "UPAICharacter.generated.h"

struct FAttributeModification;
class UNiagaraComponent;
class UUPSignificanceComponent;
class UUPActionComponent;
class UUPWorldUserWidget;
class UPawnSensingComponent;
class UPAttributeComponent;
class UUserWidget;
struct FTimerHandle;

UCLASS()
class UEPRACTICE_API AUPAICharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUPAICharacter();

public:
	virtual void PostInitializeComponents() override;

	virtual FGenericTeamId GetGenericTeamId() const override;

public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTargetActor() const;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayAttackFX();

protected:
	void OnHealthAttributeChanged(float NewValue, const FAttributeModification& AttributeModification);

	UFUNCTION()
	void OnSignificanceChanged(ESignificanceValue Significance);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Effects")
	FName AttackFX_Socket;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> AttackParticleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAudioComponent> AttackSoundComp;

	UPROPERTY(Transient)
	TObjectPtr<UUPWorldUserWidget> ActiveHealthBar;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	/* Index must match the CustomPrimitiveData index used in the Overlay material */
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	int32 HitFlash_CustomPrimitiveIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UUPActionComponent* ActionComponent;

	/* Handle fidelity for AI as they are off-screen or at far distances */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UUPSignificanceComponent> SigManComp;

	/* Key for AI Blackboard 'TargetActor' */
 	UPROPERTY(VisibleAnywhere, Category = "Effects")
 	FName TargetActorKey;

	float CachedOverlayMaxDistance;

	FTimerHandle OverlayTimerHandle;
};
