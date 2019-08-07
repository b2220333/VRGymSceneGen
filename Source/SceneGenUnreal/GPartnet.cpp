// Fill out your copyright notice in the Description page of Project Settings.

#include "GPartnet.h"

#include "json.hpp"
using json = nlohmann::json;
#include "Runtime/Core/Public/HAL/FileManager.h"
#include "Runtime/Core/Public/Misc/Paths.h"

bool AGPartnet::assignMeshFromCategoryAndAnnotationID(FString category, FString annotationID, FVector location, json::object_t params)
{
	FString path = "/Game/partnetOBJ/" + category + "/" + annotationID;
	if (assignMeshesFromPath(path, location, params)) {
		this->category = category;
		this->annotationID = annotationID;
		return true;
	}
	return false;
}

bool AGPartnet::assignRandomMeshFromCategory(FString category, FVector location, json::object_t params)
{
	IFileManager& FileManager = IFileManager::Get();
	FString path = FPaths::ProjectContentDir() + "partnetOBJ/" + category + "/*.*";
	UE_LOG(LogTemp, Warning, TEXT("Importing random mesh from %s"), *path);
	TArray<FString> modelIDs;
	FileManager.FindFiles(modelIDs, *path, false, true);
	UE_LOG(LogTemp, Warning, TEXT("Found %d models"), modelIDs.Num());
	int32 i = FMath::RandRange(0, modelIDs.Num() - 1);
	if (modelIDs.Num() > 0) {
		if (assignMeshFromCategoryAndAnnotationID(category, modelIDs[i], location, params)) {
			this->category = category;
			this->annotationID = modelIDs[i];
			return true;
		}
	}
	return false;
}