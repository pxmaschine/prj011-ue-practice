// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPInteractionComponent.h"
#include "Course/UPGameplayInterface.h"
#include "Course/UPWorldUserWidget.h"

#include "Blueprint/UserWidget.h"


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

	TraceRadius = 30.0f;
	TraceDistance = 500.0f;
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
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);

	FVector EyeLocation;
	FRotator EyeRotation;
	GetOwner()->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	const FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * TraceDistance);

	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius);

	TArray<FHitResult> Hits;
	const bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, TraceEnd, FQuat::Identity, ObjectQueryParams, Shape);

	const FColor DebugColor = bBlockingHit ? FColor::Green : FColor::Red;

	// Clear ref before trying to fill
	FocusedActor = nullptr;

	for (const FHitResult& Hit : Hits)
	{
		if (DebugDrawing::bDrawInteractionVisualize)
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, TraceRadius, 32, DebugColor, false, 2.0f);
		}

		if (AActor* HitActor = Hit.GetActor())
		{
			if (HitActor->Implements<UUPGameplayInterface>())
			{
				FocusedActor = HitActor;
				break;
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
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, DebugColor, false, 2.0f, 0, 0.0f);
	}
}
