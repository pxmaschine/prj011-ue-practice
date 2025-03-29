// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPDeveloperSettings.h"

FName UUPDeveloperSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}
