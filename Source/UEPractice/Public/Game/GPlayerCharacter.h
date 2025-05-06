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

protected:
	void Move(const FInputActionValue& Value);

	//void LookMouse(const FInputActionValue& InputValue);

protected:
	UPROPERTY(VisibleAnywhere, Category=Components)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category=Components)
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* Input_Move;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	//UInputAction* Input_LookMouse;

	UPROPERTY(EditAnywhere, Category=Movement)
	float RotationSpeed;
};
