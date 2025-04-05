// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "UPAnimInstance.generated.h"

class UUPActionComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAnimNofify_MeleeOverlap, const TArray<FOverlapResult>&);


UCLASS()
class UEPRACTICE_API UUPAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	/* Broadcast by AnimNotify_Melee anytime a melee successfully finds overlaps  */
	FOnAnimNofify_MeleeOverlap OnMeleeOverlap;

protected:
	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsStunned;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animation")
	UUPActionComponent* ActionComp;
};
