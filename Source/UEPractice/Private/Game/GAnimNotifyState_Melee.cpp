// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GAnimNotifyState_Melee.h"
#include "Game/GAnimInstance.h"

#include "Engine/OverlapResult.h"

#if !UE_BUILD_SHIPPING
namespace DevelopmentOnly
{
	static bool GGameDrawDebugMelee = false;
	static FAutoConsoleVariableRef CVarDrawDebug_MeleeNotifies(
		TEXT("game.DrawDebugMelee"),
		GGameDrawDebugMelee,
		TEXT("Enable debug rendering on the melee system.\n"),
		ECVF_Cheat
		);
}
#endif


UGAnimNotifyState_Melee::UGAnimNotifyState_Melee()
{
	Radius = 128.0f;
	TraceChannel = ECC_Pawn;
}

void UGAnimNotifyState_Melee::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	// We could run async requests here continuously any time we have new results to fetch, could stop request after the first successful 'hit'

	if (!MeshComp->GetOwner() || !MeshComp->GetOwner()->HasAuthority())
	{
		// We can skip both overlaps and callbacks for clients
		// unless we want to do some kind of empty callback anyway on clients
		return;
	}

	const FVector StartLoc = MeshComp->GetSocketLocation(SocketName);
	// assert on invalid bone name, or some other type of error throwing

	const UWorld* World = MeshComp->GetWorld();

	FCollisionShape Shape;
	Shape.SetSphere(Radius);

	// Ignore self from melee options
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MeshComp->GetOwner());

	// todo: make this async
	TArray<FOverlapResult> Overlaps;
	World->OverlapMultiByChannel(Overlaps, StartLoc, FQuat::Identity, TraceChannel, Shape, QueryParams);

#if !UE_BUILD_SHIPPING
	if (DevelopmentOnly::GGameDrawDebugMelee)
	{
		DrawDebugSphere(World, StartLoc, Radius, 32.f, FColor::Cyan, false, 0.0f);
		
		for (FOverlapResult& Result : Overlaps)
		{
			DrawDebugBox(World, Result.GetComponent()->GetComponentLocation(), FVector(32.f), FColor::Green, false, 5.0f);
		}
	}
#endif

	// Report to be handled by anyone listening (should be the Action that triggered the animation)
	if (Overlaps.Num() > 0)
	{
		if (UGAnimInstance* AnimInst = Cast<UGAnimInstance>(MeshComp->GetAnimInstance()))
		{
			AnimInst->OnMeleeOverlap.Broadcast(Overlaps);
		}
	}
}
