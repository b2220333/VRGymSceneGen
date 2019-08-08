//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GymAgent.h"
#include "GDemoAgent.generated.h"

/**
 * 
 */
UCLASS()
class SCENEGENUNREAL_API AGDemoAgent : public AGymAgent
{
	GENERATED_BODY()
	
	/*
	For MURI demo. Will use Unreal Engine default third person hero skeletal mesh.
	*/

public:
	// tries to pickup a gym object e.g. pots, pans, ingredients
	bool pickUpObject();

	// drops a object if it is holding one
	bool dropObject();

	// releases an object with an initial velocity forward
	bool throwObject(float velocity);

private:
	bool isHoldingObject;



	
	
};
