// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "UPAnimNotifyState_Melee.generated.h"


UCLASS()
class UEPRACTICE_API UUPAnimNotifyState_Melee : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UUPAnimNotifyState_Melee();

public:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> TraceChannel;

	/* Bone to center the (Sphere) Overlap around */
	UPROPERTY(EditAnywhere)
	FName SocketName;

	UPROPERTY(EditAnywhere)
	float Radius;
};
