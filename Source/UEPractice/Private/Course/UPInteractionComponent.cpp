// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPInteractionComponent.h"
#include "Course/UPGameplayInterface.h"
#include "Course/UPWorldUserWidget.h"

#include "Blueprint/UserWidget.h"


static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("up_InteractionDebugDraw"), false, TEXT("Enable debug drawing of interaction component."), ECVF_Cheat);


UUPInteractionComponent::UUPInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceRadius = 30.0f;
	TraceDistance = 500.0f;
	CollisionChannel = ECC_WorldDynamic;
}

void UUPInteractionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FindBestInteractable();
}

void UUPInteractionComponent::PrimaryInteract() const
{
	if (!FocusedActor)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("No focused actor to interact."));
		return;
	}

	APawn* MyPawn = Cast<APawn>(GetOwner());

	IUPGameplayInterface::Execute_Interact(FocusedActor, MyPawn);
}

void UUPInteractionComponent::FindBestInteractable()
{
	const bool bIsDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);

	AActor* MyOwner = GetOwner();

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	const FVector End = EyeLocation + (EyeRotation.Vector() * TraceDistance);

	//FHitResult Hit;
	//bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams);

	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius);

	TArray<FHitResult> Hits;
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);

	FColor DebugColor = bBlockingHit ? FColor::Green : FColor::Red;

	// Clear ref before trying to fill
	FocusedActor = nullptr;

	for (const FHitResult& Hit : Hits)
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			if (HitActor->Implements<UUPGameplayInterface>())
			{
				FocusedActor = HitActor;

				if (bIsDebugDraw)
				{
					DrawDebugSphere(GetWorld(), Hit.ImpactPoint, TraceRadius, 32, DebugColor, false, 2.0f);
				}

				break;
			}
		}
	}

	if (FocusedActor)
	{
		if (!DefaultWidgetInstance && ensure(DefaultWidgetClass))
		{
			DefaultWidgetInstance = CreateWidget<UUPWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}

		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->AttachedActor = FocusedActor;

			if (!DefaultWidgetInstance->IsInViewport())
			{
				DefaultWidgetInstance->AddToViewport();
			}
		}
	}
	else
	{
		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->RemoveFromParent();
		}
	}

	if (bIsDebugDraw)
	{
		DrawDebugLine(GetWorld(), EyeLocation, End, DebugColor, false, 2.0f, 0, 2.0f);
	}
}
