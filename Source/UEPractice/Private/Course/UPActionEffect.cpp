// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPActionEffect.h"
#include "Course/UPActionComponent.h"

#include "GameFramework/GameStateBase.h"


UUPActionEffect::UUPActionEffect()
{
	bAutoStart = true;
}

void UUPActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if (Duration > 0.0f)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "StopAction", Instigator);

		GetWorld()->GetTimerManager().SetTimer(DurationHandle, Delegate, Duration, false);
	}

	if (Period > 0.0f)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "ExecutePeriodicEffect", Instigator);

		GetWorld()->GetTimerManager().SetTimer(PeriodHandle, Delegate, Period, true);
	}
}

void UUPActionEffect::StopAction_Implementation(AActor* Instigator)
{
	if (GetWorld()->GetTimerManager().GetTimerRemaining(PeriodHandle) < UE_KINDA_SMALL_NUMBER)
	{
		ExecutePeriodicEffect(Instigator);
	}

	Super::StopAction_Implementation(Instigator);

	GetWorld()->GetTimerManager().ClearTimer(PeriodHandle);
	GetWorld()->GetTimerManager().ClearTimer(DurationHandle);

	UUPActionComponent* Comp = GetOwningComponent();
	Comp->RemoveAction(this);
}

float UUPActionEffect::GetTimeRemaining() const
{
	// Possibly nullptr early on if joining as a client in multiplayer (server spawns GameState and replicates the actor instance to clients)
	if (const AGameStateBase* GS = GetWorld()->GetGameState<AGameStateBase>())
	{
		const float EndTime = TimeStarted + Duration;
		return EndTime - GS->GetServerWorldTimeSeconds();	
	}

	return Duration;
}

void UUPActionEffect::ExecutePeriodicEffect_Implementation(AActor* Instigator) {}
