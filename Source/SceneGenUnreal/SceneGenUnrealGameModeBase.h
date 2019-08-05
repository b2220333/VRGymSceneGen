// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Shapenet.h"

#include "json.hpp"
using json = nlohmann::json;

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SceneGenUnrealGameModeBase.generated.h"

UCLASS()
class SCENEGENUNREAL_API ASceneGenUnrealGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASceneGenUnrealGameModeBase();


	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// spawns shapenet model actors from AShapenetClass
	void spawnShapenetActors();

	FString partnetDir = "D://data/data_v0";

	TArray<AShapenet*> shapenetActors;
	

	void randomizeTextures(AShapenet shapenetActor);

	void randomizePosition(AShapenet shapenetActor, int32 constraints);

	


	void importShapenetActorGroup(json::object_t actorGroup, FVector origin);
	float sampleLocation(json::object_t &location);
	void importShapenetActor(json::object_t actor, FVector origin);

	void listDescendants(json::object_t&  actorGroup);


	void passDownParams(json::object_t &srcObj);
	void transferParamsBetween(json::object_t &srcObj, json::object_t &dstObj);

	FDateTime roomJsonLastChanged;
	
	void resetDamping();
	bool objectsDamped;

	json parsed;


	FDateTime roomJsonTimeStamp = FDateTime(0);

	void spawnLighting();

	FTimerHandle FuzeTimerHandle;

	void spawnWalls();



	json::object_t parseJsonFromPath(FString path);
	json::array_t parseJsonArrayFromPath(FString path);

};
