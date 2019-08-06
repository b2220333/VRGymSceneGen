// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "json.hpp"
using json = nlohmann::json;

#include "CoreMinimal.h"
#include "GymObj.h"
#include "GShapenet.generated.h"

/**
 * 
 */
UCLASS()
class SCENEGENUNREAL_API AGShapenet : public AGymObj
{
	GENERATED_BODY()
	
	// imports  mesh from wordnet synset and model ID
	bool assignMeshFromSynsetAndModelID(FString synset, FString modelID, FVector location, json::object_t params);

	// imports a random mesh from wordnet synset
	bool assignRandomFromSynset(FString synset, FVector location, json::object_t params);
	
	// calls assignMeshFromPath since shapenet models only have one mesh
	virtual bool assignMeshesFromPath(FString path, FVector location, json::object_t params);

private:
	FString synset;
	FString modelID;
};
