//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "Components/ActorComponent.h"
//#include "UPAttributeComponent.generated.h"
//
//
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged, AActor*, InstigatorCator, UUPAttributeComponent*, OwningComp, float, NewValue, float, Delta);
//
//
//UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
//class UEPRACTICE_API UUPAttributeComponent : public UActorComponent
//{
//	GENERATED_BODY()
//
//public:	
//	// Sets default values for this component's properties
//	UUPAttributeComponent();
//
//public:
//	UFUNCTION(BlueprintCallable, Category = "Attributes")
//	static UUPAttributeComponent* GetAttributes(const AActor* FromActor);
//
//	/*
//	UFUNCTION(BlueprintCallable, Category = "Attributes", meta = (DisplayName = "IsAlive"))
//	static bool IsActorAlive(const AActor* FromActor);
//	*/
//	
//public:
//	UFUNCTION(BlueprintCallable, Category = "Attributes")
//	bool Kill(AActor* InstigatorActor);
//
//	UFUNCTION(BlueprintCallable, Category = "Attributes")
//	bool IsAlive() const;
//
//	UFUNCTION(BlueprintCallable, Category = "Attributes")
//	bool IsFullHealth() const;
//
//	UFUNCTION(BlueprintCallable, Category = "Attributes")
//	float GetMaxHealth() const { return MaxHealth; }
//
//	UFUNCTION(BlueprintCallable, Category = "Attributes")
//	float GetCurrentHealth() const { return Health; }
//
//	UFUNCTION(BlueprintCallable, Category = "Attributes")
//	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);
//
//	UFUNCTION(BlueprintCallable, Category = "Attributes")
//	float GetMaxRage() const { return MaxRage; }
//
//	UFUNCTION(BlueprintCallable, Category = "Attributes")
//	float GetCurrentRage() const { return Rage; }
//
//	UFUNCTION(BlueprintCallable, Category = "Attributes")
//	bool ApplyRageChange(AActor* InstigatorActor, float Delta);
//
//protected:
//	UFUNCTION(NetMulticast, Reliable)  // @FIXME: Mark as 'Unreliable' once we move the 'state' out of UPCharacter
//	void MulticastHealthChanged(AActor* InstigatorActor, float NewHealth, float Delta);
//
//	UFUNCTION(NetMulticast, Reliable)  // @FIXME: Mark as 'Unreliable' once we move the 'state' out of UPCharacter
//	void MulticastRageChanged(AActor* InstigatorActor, float NewHealth, float Delta);
//
//public:
//	UPROPERTY(BlueprintAssignable, Category = "Attributes")
//	FOnAttributeChanged OnHealthChanged;
//
//	UPROPERTY(BlueprintAssignable, Category = "Attributes")
//	FOnAttributeChanged OnRageChanged;
//
//protected:
//	// EditAnywhere - edit in BP editor and per-instance in level.
//	// VisibleAnywhere - 'read-only' in editor and level. (Use for Components)
//	// EditDefaultsOnly - hide variable per-instance, edit in BP editor only
//	// VisibleDefaultsOnly - 'read-only' access for variable, only in BP editor (uncommon)
//	// EditInstanceOnly - allow only editing of instance (eg. when placed in level)
//	// --
//	// BlueprintReadOnly - read-only in the Blueprint scripting (does not affect 'details'-panel)
//	// BlueprintReadWrite - read-write access in Blueprints
//	// --
//	// Category = "" - display only for detail panels and blueprint context menu.
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
//	float Health;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
//	float MaxHealth;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
//	float Rage;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
//	float MaxRage;
//};
