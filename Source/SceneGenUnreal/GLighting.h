// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GymObj.h"
#include "GLighting.generated.h"

/**
 * 
 */
UCLASS()
class SCENEGENUNREAL_API AGLighting : public AGymObj
{
	GENERATED_BODY()
	
	// testing add light
	/*
	FVector lightLocation = FVector(0, 0, 50);
	FRotator lightRotation = FRotator(90, 0, 0);
	FActorSpawnParameters lightSpawnParams;
	ADirectionalLight* spawnedLight = GetWorld()->SpawnActor<ADirectionalLight>(lightLocation, lightRotation, lightSpawnParams);
	spawnedLight->GetRootComponent()->SetMobility(EComponentMobility::Movable);
	*/
	
	
};
