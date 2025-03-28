// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UEPracticeDeveloperSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UEPracticeDeveloperSettings)

FName UUEPracticeDeveloperSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}
