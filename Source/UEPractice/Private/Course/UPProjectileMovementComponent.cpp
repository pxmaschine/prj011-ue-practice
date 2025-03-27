// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPProjectileMovementComponent.h"
#include "Course/UPTickManager.h"

void UUPProjectileMovementComponent::InitializeComponent()
{
	// Reset to default for object pooling support
	Velocity = FVector(1,0,0);
	
	Super::InitializeComponent();
}

void UUPProjectileMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// BeginPlay on the component will be called after the tick has been registered via the owning Actor
	UUPTickManager* TickManager = GetWorld()->GetSubsystem<UUPTickManager>();
	TickManager->RegisterComponent(&PrimaryComponentTick);
}

void UUPProjectileMovementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UUPTickManager* TickManager = GetWorld()->GetSubsystem<UUPTickManager>();
	TickManager->UnregisterComponent(&PrimaryComponentTick);
}
