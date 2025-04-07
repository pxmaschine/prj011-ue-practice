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
	static UUPActionComponent* GetActionComponent(AActor* FromActor);

	FUPAttribute* GetAttribute(FGameplayTag InAttributeTag);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Attributes, meta = (Keywords = "Add, Set"))
	bool ApplyAttributeChange(const FAttributeModification& Modification);

	/* Provide a default attribute set type for (base) classes, blueprint can set this via the details panel instead */
	void SetDefaultAttributeSet(UScriptStruct* InDefaultType);

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

	/* Stop every action, for example during death */
	void StopAllActions();

protected:
	/* Marked protected, C++ can use direct access to the OnAttributeChanged inside an Attribute */
	UFUNCTION(BlueprintCallable, DisplayName="AddAttributeListener", meta = (Keywords = "Bind, Delegate", AdvancedDisplay="bCallImmediately"))
	void K2_AddAttributeListener(FGameplayTag AttributeTag, FOnAttributeChangedDynamic Event, bool bCallImmediately = false);

	UFUNCTION(BlueprintCallable, Category=Attributes, DisplayName="GetAttribute")
	bool K2_GetAttribute(FGameplayTag InAttributeTag, float& CurrentValue, float& Base, float& Delta);

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

protected:
	UPROPERTY(EditAnywhere, Category = "Actions")
	TArray<TSubclassOf<UUPAction>> DefaultActions;

	UPROPERTY(Transient, BlueprintReadOnly, Replicated)
	TArray<UUPAction*> Actions;

	/* Interchangeable set of attributes such as Health, BaseDamage, Strength, Stamina, MoveSpeed, etc. */
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category=Attributes, meta = (BaseStruct = "/Script/UEPractice.UPAttributeSet", ExcludeBaseStruct))
	FInstancedStruct AttributeSet;

	/* Fetch from properties stored inside the AttributeSet for quick access */
	TMap<FGameplayTag, FUPAttribute*> AttributeCache;

	/* List of delegates that came from Blueprint to ensure we can clean up "dead" hooks */
	TMap<FOnAttributeChangedDynamic, FDelegateHandle> DynamicDelegateHandles;
};
