// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "UPAction.generated.h"


class UUPActionComponent;

USTRUCT()
struct FActionRepData
{
	GENERATED_BODY()

	FActionRepData()
	{
		bIsRunning = false;
		Instigator = nullptr;
	}

public:
	UPROPERTY()
	bool bIsRunning;

	UPROPERTY()
	AActor* Instigator;
};

UCLASS(Blueprintable)
class UEPRACTICE_API UUPAction : public UObject
{
	GENERATED_BODY()

public:
	virtual void BeginDestroy() override;

	virtual UWorld* GetWorld() const override;

	virtual bool IsSupportedForNetworking() const override
	{
		return true;
	}

public:
	void Initialize(UUPActionComponent* NewActionComp);

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	bool CanStart(AActor* Instigator) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	bool IsRunning() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void StopAction(AActor* Instigator);

	UFUNCTION(BlueprintPure)
	FGameplayTag GetActivationTag() const
	{
		return ActivationTag;
	}

	UFUNCTION(BlueprintPure)
	bool IsAutoStart() const
	{
		return bAutoStart;
	}

protected:
	UFUNCTION(BlueprintCallable, Category = "Action")
	UUPActionComponent* GetOwningComponent() const;
	
	UFUNCTION()
	void OnRep_RepData();

protected:
	UPROPERTY(Transient, Replicated)
	UUPActionComponent* ActionComp;

	// Tags added to OwningActor when activated, removed when action stops
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer GrantedTags;

	// Action can only start if OwningActor has none of these tags applied
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer BlockedTags;

	UPROPERTY(Transient, ReplicatedUsing="OnRep_RepData")
	FActionRepData RepData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(Transient, Replicated)
	float TimeStarted;

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	FGameplayTag ActivationTag;

	// Start immediately when added to an action component
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	bool bAutoStart;
};
