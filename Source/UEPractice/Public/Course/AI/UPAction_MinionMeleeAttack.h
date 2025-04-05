// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Course/UPAction.h"
#include "UPAction_MinionMeleeAttack.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class UEPRACTICE_API UUPAction_MinionMeleeAttack : public UUPAction
{
	GENERATED_BODY()

public:
	UUPAction_MinionMeleeAttack();

public:
	virtual void StartAction_Implementation(AActor* Instigator) override;
	
	virtual void StopAction_Implementation(AActor* Instigator) override;

protected:
	void OnMeleeOverlaps(const TArray<FOverlapResult>& Overlaps);

protected:
	UPROPERTY(EditAnywhere, Category=Animation)
	TObjectPtr<UAnimMontage> MeleeMontage;

	FDelegateHandle OnOverlapHandle;

	/* Have we already applied damage once */
	bool bDamageApplied = false;
};
