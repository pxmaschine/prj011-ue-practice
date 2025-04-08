// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Course/UPSignificanceInterface.h"
#include "GameFramework/Character.h"
#include "UPAICharacter.generated.h"

struct FAttributeModification;
class UNiagaraComponent;
class UUPActionComponent;
class UUPWorldUserWidget;
class UPawnSensingComponent;
class UUserWidget;
struct FTimerHandle;

UCLASS()
class UEPRACTICE_API AUPAICharacter : public ACharacter, public IGenericTeamAgentInterface, public IUPSignificanceInterface
{
	GENERATED_BODY()

public:
	AUPAICharacter(const FObjectInitializer& ObjectInitializer);

public:
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual void SignificanceLODChanged(int32 NewLOD) override;

public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTargetActor() const;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayAttackFX();

protected:
	void OnHealthAttributeChanged(float NewValue, const FAttributeModification& AttributeModification);

	void OnReduceAnimationWork(class USkeletalMeshComponentBudgeted* InComponent, bool bReduce);

	void OnHitFlashFinshed();

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

	/* Specifies a category for Significance Manager. Each unique Tag will have its own set of "Buckets" to sort and assign LODs based on distance etc. */
	UPROPERTY(EditDefaultsOnly, Category="Performance")
	FName SignificanceTag;

	/* Key for AI Blackboard 'TargetActor' */
 	UPROPERTY(VisibleAnywhere, Category = "Effects")
 	FName TargetActorKey;

	float CachedOverlayMaxDistance;

	FTimerHandle OverlayTimerHandle;
};
