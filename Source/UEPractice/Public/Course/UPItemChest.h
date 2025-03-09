// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UPGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "UPItemChest.generated.h"

UCLASS()
class UEPRACTICE_API AUPItemChest : public AActor, public IUPGameplayInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AUPItemChest();

public:
	void Interact_Implementation(APawn* InstigatorPawn) override;

protected:
	UFUNCTION()
	void OnRep_LipOpened();

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LidMesh{ nullptr };

	UPROPERTY(EditAnywhere)
	float TargetPitch;

	UPROPERTY(ReplicatedUsing="OnRep_LipOpened", BlueprintReadOnly)  // RepNotify
	bool bLidOpened;
};
