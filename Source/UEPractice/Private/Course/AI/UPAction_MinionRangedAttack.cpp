// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/UPAction_MinionRangedAttack.h"
#include "Course/UPActionComponent.h"
#include "Course/UPAttributeComponent.h"
#include "Course/AI/UPAICharacter.h"
#include "UEPractice/UEPractice.h"

void UUPAction_MinionRangedAttack::StartAction_Implementation(AActor* Instigator)
{
	AUPAICharacter* MyPawn = CastChecked<AUPAICharacter>(GetOwningComponent()->GetOwner());

	AActor* TargetActor = MyPawn->GetTargetActor();
	if (TargetActor == nullptr)
	{
		//return EBTNodeResult::Failed;
		return;
	}

	if (!UUPAttributeComponent::IsActorAlive(TargetActor))
	{
		//return EBTNodeResult::Failed;
		return;
	}

	const FVector MuzzleLocation = MyPawn->GetMesh()->GetSocketLocation(MeshSockets::Muzzle);
	// using TargetLocation instead of ActorLocation we *could* retrieve 'better' aiming location if implemented
	const FVector Direction = TargetActor->GetTargetLocation() - MuzzleLocation;
	FRotator MuzzleRotation = Direction.Rotation();

	// Ignore negative pitch to not hit the floor in front itself
	MuzzleRotation.Pitch += FMath::RandRange(0.0f, MaxBulletSpread);
	MuzzleRotation.Yaw += FMath::RandRange(-MaxBulletSpread, MaxBulletSpread);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Instigator = MyPawn;

	AActor* NewProj = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, Params);

	//return NewProj ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
