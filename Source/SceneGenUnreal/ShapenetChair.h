// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Shapenet.h"
#include "ShapenetChair.generated.h"

/**
 * 
 */
UCLASS()
class SCENEGENUNREAL_API AShapenetChair : public AShapenet
{
	GENERATED_BODY()

	AShapenetChair();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;


	void importModel(FString annotationID);

	FString partnetDir = "D://data/data_v0";
	TArray<FString> partnetCategories = { "Chair" };

	FString mainPartnetCategory = "Chair";


};
