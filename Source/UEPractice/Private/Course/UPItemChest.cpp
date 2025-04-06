// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPItemChest.h"
#include "Course/UPTweenSubsystem.h"

#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Net/UnrealNetwork.h"


AUPItemChest::AUPItemChest()
{
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	RootComponent = BaseMesh;
	BaseMesh->SetSimulatePhysics(true);

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>("LidMesh");
	LidMesh->SetupAttachment(BaseMesh);

	OpenChestEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("OpeningEffectComp"));
	OpenChestEffect->SetupAttachment(RootComponent);
	OpenChestEffect->bAutoActivate = false;
	// only attach while playing the VFX, this skips transform updates when the chests moves around the world
	// while the VFX is not active
	OpenChestEffect->bAutoManageAttachment = true;

	// If the chest was non-movable we could just call "playsoundatlocation" and skip creating a component during spawn
	OpenChestSound = CreateDefaultSubobject<UAudioComponent>(TEXT("OpenChestSFX"));
	OpenChestSound->SetupAttachment(RootComponent);
	OpenChestSound->SetAutoActivate(false);
	OpenChestSound->bAutoManageAttachment = true;

	bLidOpened = false;

	bReplicates = true;

	//PrimaryActorTick.bCanEverTick = true;
}

void AUPItemChest::OnActorLoaded_Implementation()
{
	ConditionalOpenChest();
}

void AUPItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUPItemChest, bLidOpened);
}

void AUPItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	bLidOpened = true;
	ConditionalOpenChest();
}

void AUPItemChest::ConditionalOpenChest()
{
	if (bLidOpened)
	{
		UUPTweenSubsystem* AnimSubsystem = GetWorld()->GetSubsystem<UUPTweenSubsystem>();
		
		// Several ways to trigger and manage these animations (curve-based automatic ticking, manual ticking, and easing functions)
		/*
		AnimSubsystem->PlayTween(LidAnimCurve, 1.0f, [&](float CurrValue)
		{
			LidMesh->SetRelativeRotation(FRotator(CurrValue, 0, 0));
		});*/

		// manually handled variation to tick yourself
		/*CurveAnimInst = new FActiveCurveAnim(LidAnimCurve, [&](float CurrValue)
		{
			LidMesh->SetRelativeRotation(FRotator(CurrValue, 0, 0));
		}, 1.0f);*/

		AnimSubsystem->PlayEasingFunc(EEasingFunc::EaseInOut, 2.0f, 2.0f, [&](float CurrValue)
		{
			LidMesh->SetRelativeRotation(FRotator(CurrValue * 100.f, 0, 0));
		});

		OpenChestEffect->Activate(true);

		OpenChestSound->Play();
	}
}

void AUPItemChest::OnRep_LidOpened()
{
	ConditionalOpenChest();
}

/*
void ARogueTreasureChest::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Example of manually ticking the animation, may be useful if you need the control and/or manually batch the anims
	if (CurveAnimInst && CurveAnimInst->IsValid())
	{
		CurveAnimInst->Tick(DeltaSeconds);
	}
}*/
