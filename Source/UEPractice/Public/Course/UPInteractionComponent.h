// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UPInteractionComponent.generated.h"


class UUPWorldUserWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UEPRACTICE_API UUPInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UUPInteractionComponent();

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void PrimaryInteract();

protected:
	// Reliable - Will always arrive, eventually. Request will be re-sent unless an acknowledgement was received.
	// Unreliable - Not guaranteed, packet can get lost and won't retry.
	UFUNCTION(Server, Reliable)
	// When passing a pointer to a RPC, Unreal Engine will convert it to an Actor ID and send that to the remote
	void ServerInteract(AActor* InFocus);

	void FindBestInteractable();

protected:
	UPROPERTY(Transient)
	AActor* FocusedActor;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUPWorldUserWidget> DefaultWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UUPWorldUserWidget> WidgetInst;
};
