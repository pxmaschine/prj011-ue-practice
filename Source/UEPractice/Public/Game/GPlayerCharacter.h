// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GPlayerCharacter.generated.h"

class UInputMappingContext;
struct FInputActionValue;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class UNiagaraSystem;
class USoundBase;
class UUPActionComponent;
class UUPWorldUserWidget;

UCLASS()
class UEPRACTICE_API AGPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	//virtual FVector GetPawnViewLocation() const override;

public:
	void PlayAttackSound(USoundBase* InSound);

	AActor* GetTargetActor() const { return TargetActor; }

protected:
	void Move(const FInputActionValue& Value);

	//void LookMouse(const FInputActionValue& InputValue);

	void PrimaryAttack();

	//void AttackDelay_Elapsed();

protected:
	UPROPERTY(VisibleAnywhere, Category = Components)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category = Components)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	TObjectPtr<UAudioComponent> AttackSoundsComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UUPActionComponent* ActionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* Input_Move;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	//UInputAction* Input_LookMouse;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	//UInputAction* Input_PrimaryAttack;

	UPROPERTY(EditAnywhere, Category = Movement)
	float RotationSpeed;

	///* Sphere radius of the sweep to find desired target under crosshair. Adjusts final projectile direction */
	//UPROPERTY(EditAnywhere, Category = Targeting)
	//float SweepRadius;

	///* Fallback distance when sweep finds no collision under crosshair. Adjusts final projectile direction */
	//UPROPERTY(EditAnywhere, Category = Targeting)
	//float SweepDistanceFallback;

	UPROPERTY(EditAnywhere, Category = Attack)
	float PrimaryAttackDelay;

	//UPROPERTY(EditAnywhere, Category = Attack)
	//TSubclassOf<AActor> ProjectileClass;

	//UPROPERTY(VisibleAnywhere, Category = Effects)
	//FName ProjectileSocketName;

	//UPROPERTY(EditDefaultsOnly, Category = Attack)
	//float AttackAnimDelay;

	//UPROPERTY(EditAnywhere, Category = Attack)
	//UAnimMontage* AttackAnim;

	//UPROPERTY(EditAnywhere, Category = Attack)
	//UNiagaraSystem* CastVFX;

	///* Sound Effect to play (Can be Wave or Cue) */
	//UPROPERTY(EditAnywhere, Category = Attack)
	//TObjectPtr<USoundBase> CastingSound;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UUPWorldUserWidget> ActiveHealthBar;

	UPROPERTY(Transient)
	AActor* TargetActor;
};
