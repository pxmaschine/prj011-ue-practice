// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GAnimInstance.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnAnimNofify_MeleeOverlap, const TArray<FOverlapResult>&);

class UUPActionComponent;

UCLASS()
class UEPRACTICE_API UGAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	/* Broadcast by AnimNotify_Melee anytime a melee successfully finds overlaps  */
	FOnAnimNofify_MeleeOverlap OnMeleeOverlap;

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animation")
	UUPActionComponent* ActionComp;
};
