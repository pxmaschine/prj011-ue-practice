// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPInteractionComponent.h"
#include "Course/UPGameplayInterface.h"
#include "Course/UPWorldUserWidget.h"

#include "Blueprint/UserWidget.h"
#include "Engine/OverlapResult.h"


namespace DebugDrawing
{
	static bool bDrawInteractionVisualize = false;
	static FAutoConsoleVariableRef CVarDebugDrawInteraction(TEXT("up.InteractionDebugDraw"),
		bDrawInteractionVisualize,
		TEXT("Enable Debug Lines for Interaction Component."),
		ECVF_Cheat);
}

UUPInteractionComponent::UUPInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// Since we use Camera info in Tick we want the most up-to-date camera position for tracing
	PrimaryComponentTick.TickGroup = TG_PostUpdateWork;

	TraceRadius = 250.f;
	CollisionChannel = ECC_WorldDynamic;
}

void UUPInteractionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Cast checked acts like static_cast in shipping builds. Less overhead compared to regular Cast<T> which does have safety nets.
	// Can use this in places where the cast object should never be nullptr by design and we know exactly the base class it is.
	const APawn* MyPawn = CastChecked<APawn>(GetOwner());
	if (MyPawn->IsLocallyControlled()) // Todo: Ideally just disable tick on this component when owner is not locally controlled.
	{
		FindBestInteractable();
	}
}

void UUPInteractionComponent::PrimaryInteract()
{
	ServerInteract(FocusedActor);
}

void UUPInteractionComponent::ServerInteract_Implementation(AActor* InFocus)
{
	if (!InFocus)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("No focused actor to interact."));
		return;
	}

	APawn* MyPawn = CastChecked<APawn>(GetOwner());

	IUPGameplayInterface::Execute_Interact(InFocus, MyPawn);
}

void UUPInteractionComponent::FindBestInteractable()
{
	const UWorld* World = GetWorld();

	const FVector TraceOrigin = GetOwner()->GetActorLocation();

	// Find all potential interactables around the player
	TArray<FOverlapResult> Overlaps;
	World->OverlapMultiByChannel(
		Overlaps,
		TraceOrigin,
		FQuat::Identity,
		CollisionChannel,
		FCollisionShape::MakeSphere(TraceRadius));

	const FColor LineColor = FColor::Green;
	if (DebugDrawing::bDrawInteractionVisualize)
	{
		DrawDebugSphere(World, TraceOrigin, TraceRadius, 32, LineColor, false, 0.0f);
	}

	const APawn* OwningPawn = Cast<APawn>(GetOwner());
	const AController* OwningController = OwningPawn->GetController();
	check(OwningController); // We already check if locally controlled earlier

	FocusedActor = nullptr;
	float HighestWeight = -MAX_flt;

	// Calc 'weights' to find the best interactable which the player most likely intends to focus
	for (const FOverlapResult& Overlap : Overlaps)
	{
		if (AActor* HitActor = Overlap.GetActor())
		{
			if (DebugDrawing::bDrawInteractionVisualize)
			{
				DrawDebugSphere(GetWorld(), HitActor->GetActorLocation(),
					32, 16, LineColor, false, 0.0f);
			}

			if (HitActor->Implements<UUPGameplayInterface>())
			{
				// When not recently rendered, the player is unlikely to want to interact with this object
				// it might be hidden behind walls (shadow rendering might mess with this boolean)
				if (!HitActor->WasRecentlyRendered(0.0f))
				{
					// Won't work for Nanite meshes which don't report this recently rendered
					// - Extra: if recently rendered, then perform line trace from camera to obj pivot
					//				to know if we are actually behind some type of wall
					continue;
				}

				FVector ObjectDir = (HitActor->GetActorLocation() - TraceOrigin).GetUnsafeNormal();

				// Prefer actors that our 'control rotation' (eg. camera) is pointing to
				float DotResult = FVector::DotProduct(ObjectDir, OwningController->GetControlRotation().Vector());
				float Weight = DotResult * 10.f;
				
				if (HighestWeight < Weight)
				{
					FocusedActor = HitActor;
					HighestWeight = Weight;
				}
			}
		}
	}

	if (FocusedActor)
	{
		if (WidgetInst == nullptr && ensure(DefaultWidgetClass))
		{
			WidgetInst = CreateWidget<UUPWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}

		if (WidgetInst)
		{
			WidgetInst->AttachedActor = FocusedActor;

			if (!WidgetInst->GetParent())
			{
				UUPWorldUserWidget::AddToRootCanvasPanel(WidgetInst);
			}
		}
	}
	else
	{
		if (WidgetInst)
		{
			WidgetInst->RemoveFromParent();
		}
	}

	if (DebugDrawing::bDrawInteractionVisualize)
	{
		if (FocusedActor)
		{
			DrawDebugBox(GetWorld(), FocusedActor->GetActorLocation(), FVector(20.f), LineColor, false, 0.0f);
		}
		//DrawDebugLine(GetWorld(), TraceOrigin, TraceEnd, LineColor, false, 2.0f, 0, 0.0f);
	}
}
