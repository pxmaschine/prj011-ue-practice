// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
// Added here to more easily include whenever we also use LogGame (structuredlog is new in 5.2)
#include "Logging/StructuredLog.h"
#include "Course/UPTypes.h"

// Define category "LogGame"
UEPRACTICE_API DECLARE_LOG_CATEGORY_EXTERN(LogGame, Log, All);

// Define alias to easily keep track of custom channels in C++ (must match what is specified in Project Settings > Collision 
#define COLLISION_PROJECTILE ECC_GameTraceChannel1
#define TRACE_INTERACT ECC_GameTraceChannel2

// Perception System simple Team Numbers
#define TEAM_ID_BOTS 1
#define TEAM_ID_PLAYERS 2

namespace Collision
{
	static FName Ragdoll_ProfileName = FName(TEXT("Ragdoll"));
	static FName Projectile_ProfileName = FName(TEXT("Projectile"));
	static FName Pickup_ProfileName = FName(TEXT("Pickup"));
}

namespace MeshSockets
{
	static FName Muzzle = FName(TEXT("Muzzle_01"));
}

static void LogOnScreen(const UObject* WorldContext, const FString& Msg, const FColor Color = FColor::White, const float Duration = 5.0f)
{
	if (!ensure(WorldContext))
	{
		return;
	}

	const UWorld* World = WorldContext->GetWorld();
	if (!ensure(World))
	{
		return;
	}

	const FString NetPrefix = World->IsNetMode(NM_Client) ? "[CLIENT]" : "[SERVER]";
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, NetPrefix + Msg);
	}
}
