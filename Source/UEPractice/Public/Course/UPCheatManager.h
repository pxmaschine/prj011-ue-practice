// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "UPCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class UEPRACTICE_API UUPCheatManager : public UCheatManager
{
	GENERATED_BODY()

	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void HealSelf(float Amount = 100);
	
	UFUNCTION(Exec, BlueprintAuthorityOnly)
	void KillAll();
	
	UFUNCTION(Exec)
	void DeleteSaveGame();
};
