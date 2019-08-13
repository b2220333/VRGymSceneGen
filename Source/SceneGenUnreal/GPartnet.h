// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GymObj.h"
#include "GPartnet.generated.h"

/**
 * 
 */
UCLASS()
class SCENEGENUNREAL_API AGPartnet : public AGymObj
{
	GENERATED_BODY()

public:
	// assigns mesh from partnet category and annotation id
	bool assignMeshFromCategoryAndAnnotationID(FString category, FString annotationID, FVector location, json::object_t params);

	// assigns random mesh from partnet category
	bool assignRandomMeshFromCategory(FString category, FVector location, json::object_t params);
	
private:
	FString category;
	FString annotationID;
};
