// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GymObj.h"
#include "GWall.generated.h"

/**
 * 
 */
UCLASS()
class SCENEGENUNREAL_API AGWall : public AGymObj
{
	GENERATED_BODY()

public:
	// spawns wall at with world transform
	bool spawnWall(FTransform wallTransform);

	// spawn floor with x width, y length centered at (0,0,0)
	bool spawnFloor(float xWidth, float yWidth);

	// temporary: makes all walls wood
	bool applyDemoWallParams();

private:
	FString woodMaterialPath = "/Game/DemoWood/woodMat.woodMat";
	FString woodTexturePath = "/Game/DemoWood/woodTexture.woodTexture";


};
