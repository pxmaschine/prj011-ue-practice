// Fill out your copyright notice in the Description page of Project Settings.


#include <Game/GGameplayFunctionLibrary.h>
#include <Game/GGameModeBase.h>

AActor* UGGameplayFunctionLibrary::GetClosestEnemy(const AActor* FromActor)
{
	if (!FromActor)
	{
		return nullptr;
	}

	if (AGGameModeBase* GM = FromActor->GetWorld()->GetAuthGameMode<AGGameModeBase>())
	{
		const FVector From = FromActor->GetActorLocation();
		AActor* Closest = nullptr;
		float MinDistance = UE_MAX_FLT;

		for (AActor* Enemy : GM->GetActiveEnemies())
		{
			float EnemyDistance = FVector::Distance(From, Enemy->GetActorLocation());
			if (EnemyDistance < MinDistance)
			{
				MinDistance = EnemyDistance;
				Closest = Enemy;
			}
		}

		return Closest;
	}

	return nullptr;
}
