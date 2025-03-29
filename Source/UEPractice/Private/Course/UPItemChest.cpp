// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPItemChest.h"
#include "Course/UPTweenSubsystem.h"

#include "Net/UnrealNetwork.h"


AUPItemChest::AUPItemChest()
{
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>("LidMesh");
	LidMesh->SetupAttachment(BaseMesh);

	bLidOpened = false;

	bReplicates = true;
}

void AUPItemChest::OnActorLoaded_Implementation()
{
	if (bLidOpened)
	{
		OpenChest();
	}
}

void AUPItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUPItemChest, bLidOpened);
}

void AUPItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	bLidOpened = true;

	if (bLidOpened)
	{
		OpenChest();
	}
}

void AUPItemChest::OpenChest()
{
	// @todo: lidmesh still as replicated relative rotation?
	UUPTweenSubsystem* AnimSubsystem = GetWorld()->GetSubsystem<UUPTweenSubsystem>();
	AnimSubsystem->PlayTween(LidAnimCurve, 1.0f, [&](float CurrValue)
	{
		LidMesh->SetRelativeRotation(FRotator(CurrValue, 0, 0));
	});
}

void AUPItemChest::OnRep_LidOpened()
{
	if (bLidOpened)
	{
		OpenChest();
	}
}
