// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "UPAction.generated.h"


class UUPActionComponent;

UCLASS(Blueprintable)
class UEPRACTICE_API UUPAction : public UObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	bool CanStart(AActor* Instigator) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	bool IsRunning() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void StopAction(AActor* Instigator);

protected:
	UFUNCTION(BlueprintCallable, Category = "Action")
	UUPActionComponent* GetOwningComponent() const;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	FName ActionName;

protected:
	// Tags added to OwningActor when activated, removed when action stops
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer GrantedTags;

	// Action can only start if OwningActor has none of these tags applied
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer BlockedTags;

	bool bIsRunning;
};
