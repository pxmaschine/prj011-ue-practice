// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPLoadingScreenSubsystem.h"
#include "MoviePlayer.h"


void UUPLoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Show simple loading screen when transitioning between maps
	if (!IsRunningDedicatedServer())
	{
		FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UUPLoadingScreenSubsystem::BeginLoadingScreen);
		FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UUPLoadingScreenSubsystem::EndLoadingScreen);
	}
}

void UUPLoadingScreenSubsystem::BeginLoadingScreen(const FString& MapName)
{
	FLoadingScreenAttributes LoadingScreen;

	// Don't autocomplete so we can wait for PSO compilation to finish
	//LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;

	LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

void UUPLoadingScreenSubsystem::EndLoadingScreen(UWorld* InLoadedWorld)
{
}
