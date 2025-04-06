// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Course/UPAttributeSet.h"
#include "Components/ActorComponent.h"
#include "StructUtils/InstancedStruct.h"
#include "UPActionComponent.generated.h"


class UUPAction;
class UUPActionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActionStateChanged, UUPActionComponent*, OwningComp, UUPAction*, Action);

UCLASS(ClassGroup=(UPGame), meta=(BlueprintSpawnableComponent))
class UEPRACTICE_API UUPActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UUPActionComponent();

protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	bool GetAttribute(FGameplayTag InAttributeTag, FUPAttribute& OutAttribute);

	UFUNCTION(BlueprintCallable, Category=Attributes, DisplayName="GetAttribute")
	bool K2_GetAttribute(FGameplayTag InAttributeTag, float& CurrentValue, float& Base, float& Delta);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Attributes, meta = (Keywords = "Add, Set"))
	bool ApplyAttributeChange(FGameplayTag InAttributeTag, FAttributeModification Modification);

	void BroadcastAttributeListener(FGameplayTag AttributeTag, float NewValue, const FAttributeModification& AppliedMod);

	void K2_AddAttributeListener(FGameplayTag AttributeTag, const FOnAttributeChangedDynamic& Event);

	FDelegateHandle AddAttributeListener(FGameplayTag AttributeTag, const FOnAttributeChangedNonDynamic& Func);

	void RemoveAttributeListener(FGameplayTag AttributeTag, FDelegateHandle Handle);
	
	void RemoveAttributeListener(FGameplayTag AttributeTag, FAttributeDelegateHandle Handle);

	UFUNCTION(BlueprintCallable)
	static UUPActionComponent* GetComponent(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void AddAction(AActor* Instigator, TSubclassOf<UUPAction> ActionClass);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void RemoveAction(UUPAction* Action);

	/* Returns first occurance of action matching the class provided */
 	UFUNCTION(BlueprintCallable, Category = "Actions")
 	UUPAction* GetAction(TSubclassOf<UUPAction> ActionClass) const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool StartActionByName(AActor* Instigator, FGameplayTag ActionName);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool StopActionByName(AActor* Instigator, FGameplayTag ActionName);

protected:
	UFUNCTION(Server, Reliable)
	void ServerStartAction(AActor* Instigator, FGameplayTag ActionName);

	UFUNCTION(Server, Reliable)
	void ServerStopAction(AActor* Instigator, FGameplayTag ActionName);

public:
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer ActiveGameplayTags;

	UPROPERTY(BlueprintAssignable)
	FOnActionStateChanged OnActionStarted;

	UPROPERTY(BlueprintAssignable)
	FOnActionStateChanged OnActionStopped;

	// Keep a list of delegates per unique gameplaytag
	TMap<FGameplayTag, FOnAttributeChangedList> AttributeListeners;

	TMap<FGameplayTag, TArray<FAttributeDelegateHandle>> Listeners;

protected:
	UPROPERTY(EditAnywhere, Category = "Actions")
	TArray<TSubclassOf<UUPAction>> DefaultActions;

	UPROPERTY(Transient, BlueprintReadOnly, Replicated)
	TArray<UUPAction*> Actions;

	/* Interchangeable set of attributes such as Health, BaseDamage, Strength, Stamina, MoveSpeed, etc. */
	UPROPERTY(EditAnywhere, Category=Attributes, meta = (BaseStruct = "/Script/UEPractice.UPAttributeSet", ExcludeBaseStruct))
	FInstancedStruct AttributeSet;

	/* Fetch from properties stored inside the AttributeSet for quick access */
	TMap<FGameplayTag, const FUPAttribute*> AttributeCache;
};
