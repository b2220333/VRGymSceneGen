// Fill out your copyright notice in the Description page of Project Settings.

#include "GShapenet.h"
#include "Runtime/Core/Public/HAL/FileManager.h"
#include "Runtime/Core/Public/Misc/Paths.h"

#include "json.hpp"
using json = nlohmann::json;



bool AGShapenet::assignMeshFromSynsetAndModelID(FString synset, FString modelID, FVector location, json::object_t params)
{
	FString path = "/Game/ShapenetObj/" + synset + "/" + modelID + "/model_normalized.model_normalized";
	if (assignMeshFromPath(path, location, params)) {
		this->synset = synset;
		this->modelID = modelID;
		return true;
	}
	return false;
}

bool AGShapenet::assignRandomFromSynset(FString synset, FVector location, json::object_t params)
{
	IFileManager& FileManager = IFileManager::Get();
	FString path = FPaths::ProjectContentDir() + "shapenetOBJ/" + synset + "/*.*";
	UE_LOG(LogTemp, Warning, TEXT("Importing random mesh from %s"), *path);
	TArray<FString> modelIDs;
	FileManager.FindFiles(modelIDs, *path, false, true);
	UE_LOG(LogTemp, Warning, TEXT("Found %d models"), modelIDs.Num());
	int32 i = FMath::RandRange(0, modelIDs.Num() - 1);
	if (modelIDs.Num() > 0) {
		if (assignMeshFromSynsetAndModelID(synset, modelIDs[i], location, params)) {
			this->synset = synset;
			this->modelID = modelIDs[i];
			return true;
		}
	}
	return false;
}