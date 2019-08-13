// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/Engine/Classes/Components/LightComponent.h"
#include "Runtime/Engine/Classes/Engine/Light.h"
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
	
public:
	// spawns a point light
	bool spawnPointLight(FVector location);

	// spawns a directional lights
	bool spawnDirectionalLight(FVector location, FRotator rotation);


	// getter for lightType
	FString getLightType();

	// getter for light
	ULightComponent* getLight();

private:
	FString lightType = "";
	ULightComponent* light;


	
};
