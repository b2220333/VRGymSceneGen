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


	bool spawnWall(float xWidth, float yWidth, float zWidth, FTransform wallTransform);
	bool spawnFloor(float xWidth, float yWidth);

};
