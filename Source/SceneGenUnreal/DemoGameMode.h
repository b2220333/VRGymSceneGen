// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GDemoAgent.h"

#include "CoreMinimal.h"
#include "SceneGenUnrealGameModeBase.h"
#include "DemoGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SCENEGENUNREAL_API ADemoGameMode : public ASceneGenUnrealGameModeBase
{
	GENERATED_BODY()

	/*

	Demo game mode for MURI

	Three main tasks of focus:

	1) Cooking
		- Transfer knowledge of heat -> cooking. Cooking in diffferent types of kitchens.
	2) Making drinks
		- Transfer knowledge of making drinks. Making drinks from mixing ingredients/ liquids, using machines, manual mixers

	3) Washing Dishes
		- Transfer knowledge of water + friction -> cleaning dishes. Washing from diffrent water sources.
	*/

public:

	// gives agent large posiitve reward for cooking, negative reward for overcooking, and small negative reward otherwise
	void distributeReward(AGDemoAgent* agent);

private:
	TArray<AGymObj*> ingredients;

	AGDemoAgent* demoAgent;


	// mode is either "cook" "dishes" or "drink"
	FString mode = "cook";
	
};
