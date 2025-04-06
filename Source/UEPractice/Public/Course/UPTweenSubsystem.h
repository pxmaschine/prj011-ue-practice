// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "UPTweenSubsystem.generated.h"


USTRUCT()
struct FActiveTweenData
{
	GENERATED_BODY()

	FActiveTweenData() {}

	FActiveTweenData(UCurveFloat* InCurve, TFunction<void (float)> InCallback, float InRate = 1.0f)
	{
		Curve = InCurve;
		Callback = InCallback;
		PlayRate = InRate;

		// We use first keyframe as start time, you could choose to always start at 0.0f
		Curve->GetTimeRange(CurrentTime, MaxTime);
	}

	UPROPERTY()
	UCurveFloat* Curve = nullptr;

	float PlayRate = 1.0f;
	
	/* Current time along curve */
	float CurrentTime = 0.0f;

	/* Cached max time to know when we finished */
	float MaxTime = 0.0f;

	TFunction<void(float)> Callback;

	void Tick(float DeltaTime)
	{
		CurrentTime += (DeltaTime * PlayRate);

		const float CurrentValue = Curve->GetFloatValue(CurrentTime);

		Callback(CurrentValue);

		if (CurrentTime >= MaxTime)
		{
			// Mark as "Finished", will be cleaned up by subsystem
			Curve = nullptr;
		}
	}

	bool IsFinished() const
	{
		return Curve == nullptr;
	}
};


USTRUCT()
struct FActiveEasingFunc
{
	GENERATED_BODY()

	FActiveEasingFunc() {}

	FActiveEasingFunc(const float InEasingExp, const float InPlayRate, const TFunction<void (float)>& InCallback)
		: EasingExp(InEasingExp), PlayRate(InPlayRate), Callback(InCallback) 	{}

	float EasingExp = 2.0f;
	
	float PlayRate = 1.0f;
	
	float Time = 0.0f;

	TFunction<void(float)> Callback;

	void Tick(const float DeltaTime)
	{
		Time += (DeltaTime*PlayRate);

		// Clamp
		FMath::Min(Time, 1.0f);

		const float CurrentValue = FMath::InterpEaseInOut(0.0f, 1.0f, Time, EasingExp);

		Callback(CurrentValue);
	}

	bool IsFinished() const
	{
		return Time >= 1.0f;;
	}
};

UCLASS()
class UEPRACTICE_API UUPTweenSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	// This could be a FInstancedStruct array if we want a variety of struct options with different sets of data
	TArray<FActiveTweenData> ActiveAnims;

	TArray<FActiveEasingFunc> ActiveEasingFuncs;

	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;

public:
	/* Start animation based on curve */
	void PlayTween(UCurveFloat* InCurveAsset, float InPlayRate, const TFunction<void(float)>& Func);

	/* Play easing function with output between 0.0 - 1.0 */
	void PlayEasingFunc(EEasingFunc::Type EasingType, float EasingExp, float InPlayRate, const TFunction<void(float)>& Func);
};
