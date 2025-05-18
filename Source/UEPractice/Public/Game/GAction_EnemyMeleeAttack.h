// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Course/UPAction.h"
#include "GAction_EnemyMeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class UEPRACTICE_API UGAction_EnemyMeleeAttack : public UUPAction
{
	GENERATED_BODY()
	
public:
	UGAction_EnemyMeleeAttack();

public:
	virtual void StartAction_Implementation(AActor* Instigator) override;
	
	virtual void StopAction_Implementation(AActor* Instigator) override;

	//virtual void BeginDestroy() override;

protected:
	void OnMeleeOverlaps(const TArray<FOverlapResult>& Overlaps);

protected:
	UPROPERTY(EditAnywhere, Category=Animation)
	TObjectPtr<UAnimMontage> MeleeMontage;

	FDelegateHandle OnOverlapHandle;

	/* Have we already applied damage once */
	bool bDamageApplied = false;

	//FTimerHandle AnimCompleteHandle;
};
