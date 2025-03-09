// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPItemChest.h"

#include "Net/UnrealNetwork.h"


AUPItemChest::AUPItemChest()
{
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>("LidMesh");
	LidMesh->SetupAttachment(BaseMesh);

	TargetPitch = 110.0f;
	bLidOpened = false;

	bReplicates = true;
}

void AUPItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUPItemChest, bLidOpened);
}

void AUPItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	bLidOpened = !bLidOpened;

	OnRep_LipOpened();
}

void AUPItemChest::OnRep_LipOpened()
{
	const float CurrentPitch = bLidOpened ? TargetPitch : 0.0f;
	LidMesh->SetRelativeRotation(FRotator(CurrentPitch, 0.0, 0.0));
}
