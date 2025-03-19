// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Course/UPAction.h"
#include "UPActionEffect.generated.h"

/**
 * 
 */
UCLASS()
class UEPRACTICE_API UUPActionEffect : public UUPAction
{
	GENERATED_BODY()

public:
	UUPActionEffect();

public:
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Action")
	float GetTimeRemaining() const;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Effect")
	void ExecutePeriodicEffect(AActor* Instigator);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	float Duration;

	// Time between ticks to apply effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	float Period;

	FTimerHandle DurationHandle;
	FTimerHandle PeriodHandle;
};
