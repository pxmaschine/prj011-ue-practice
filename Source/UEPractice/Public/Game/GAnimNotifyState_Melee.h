// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GAnimNotifyState_Melee.generated.h"


UCLASS()
class UEPRACTICE_API UGAnimNotifyState_Melee : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UGAnimNotifyState_Melee();

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
