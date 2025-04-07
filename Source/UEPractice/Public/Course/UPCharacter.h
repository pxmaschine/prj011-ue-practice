// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "UPCharacter.generated.h"


struct FAttributeModification;
class AUPAICharacter;
class UAIPerceptionStimuliSourceComponent;
class UUPActionComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UUPInteractionComponent;
class UAnimMontage;
class AUPProjectileBase;
class UUPAttributeComponent;
struct FInputActionValue;
struct FTimerHandle;

UCLASS()
class UEPRACTICE_API AUPCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUPCharacter();

public:
	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	virtual FVector GetPawnViewLocation() const override;

	virtual FGenericTeamId GetGenericTeamId() const override;

public:
	UFUNCTION(Client, Unreliable)
	void ClientOnSeenBy(AUPAICharacter* SeenByPawn);

	void PlayAttackSound(USoundBase* InSound);

protected:
	void Move(const FInputActionValue& Value);

	void LookMouse(const FInputActionValue& InputValue);

	void LookStick(const FInputActionValue& InputValue);

	void SprintStart();

	void SprintStop();

	void PrimaryAttack();

	void SecondaryAttack();

	void Dash();

	void PrimaryInteract();

	UFUNCTION()
	void OnHealthAttributeChanged(float NewValue, const FAttributeModification& AttributeModification);

	void FindCrosshairTarget();

	void CrosshairTraceComplete(const FTraceHandle& InTraceHandle, FTraceDatum& InTraceDatum);

protected:
	/* Index must match the CustomPrimitiveData index used in the Overlay material */
	UPROPERTY(VisibleAnywhere, Category=Effects)
	int32 HitFlash_CustomPrimitiveIndex;

	UPROPERTY(VisibleAnywhere, Category=Components)
	USpringArmComponent* SpringArmComp;

	/* Widget to display when bot first sees a player. */
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUserWidget> SpottedWidgetClass;

	//UPROPERTY(EditDefaultsOnly, Category= Effects)
	//TObjectPtr<USoundBase> TakeDamageVOSound;
	//
	//UPROPERTY(EditDefaultsOnly, Category= Effects)
	//TObjectPtr<USoundBase> DeathVOSound;

	//UPROPERTY(EditDefaultsOnly, Category= Effects)
	//TObjectPtr<USoundBase> DeathUISound;

	//UPROPERTY(EditDefaultsOnly, Category= Effects)
	//TObjectPtr<USoundBase> FootPlantSound;

	/* Plays on DefaultSlot, make sure the "auto blendout" is disabled on the AnimSequence to keep the final pose  */
	UPROPERTY(EditDefaultsOnly, Category= Effects)
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(VisibleAnywhere, Category=Components)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, Category=Components)
	UUPInteractionComponent* InteractionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components)
	UUPActionComponent* ActionComponent;

	UPROPERTY(VisibleAnywhere, Category=Components)
	TObjectPtr<UAIPerceptionStimuliSourceComponent> PerceptionStimuliComp;

	/* Re-usable audio component for all (attached) attack sounds such as casting sound from the magic projectile attack */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components)
	TObjectPtr<UAudioComponent> AttackSoundsComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* Input_Move;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* Input_LookMouse;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* Input_LookStick;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* Input_Sprint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* Input_PrimaryAttack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* Input_SecondaryAttack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* Input_Dash;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* Input_Jump;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* Input_Interact;

private:
	bool bHasPawnTarget;

	FTraceHandle TraceHandle;

	float CachedOverlayMaxDistance;

	FTimerHandle OverlayTimerHandle;
};
