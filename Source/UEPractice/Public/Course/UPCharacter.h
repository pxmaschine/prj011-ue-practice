// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UPCharacter.generated.h"


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
class UEPRACTICE_API AUPCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	/* VisibleAnywhere = read-only, still useful to view in-editor and enforce a convention. */
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParamName;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName HandSocketName;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AUPProjectileBase> MagicProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AUPProjectileBase> BlackholeProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AUPProjectileBase> DashProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Attack")
	UParticleSystem* CastVFX{ nullptr };

	FTimerHandle TimerHandle_PrimaryAttack;
	FTimerHandle TimerHandle_BlackholeAttack;
	FTimerHandle TimerHandle_Dash;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AttackAnimDelay;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp{ nullptr };

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp{ nullptr };

	UPROPERTY(VisibleAnywhere)
	UUPInteractionComponent* InteractionComp{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UUPAttributeComponent* AttributeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PrimaryAttackAction{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* UltimateAttackAction{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction{ nullptr };

public:
	// Sets default values for this character's properties
	AUPCharacter();

protected:
	virtual void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

public:
	// Exec works for certain classes - PlayerController, GameMode, CheatManager
	// Adds function automatic to console commands
	UFUNCTION(Exec)
	void HealSelf(float Amount = 100.0f);

protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void PrimaryAttack();
	void PrimaryAttack_TimeElapsed();
	void BlackholeAttack();
	void BlackholeAttack_TimeElapsed();
	void Dash();
	void Dash_TimeElapsed();

	void StartAttackEffects();

	void SpawnProjectile(TSubclassOf<AUPProjectileBase> ProjectileClass);

	void PrimaryInteract();

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, UUPAttributeComponent* OwningComp, float NewHealth, float Delta);
};
