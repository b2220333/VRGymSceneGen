// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GymAgent.h"

#include "json.hpp"
using json = nlohmann::json;

#include "GymObj.h"

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
	FString shapenetDir = "D://data/ShapeNetCore.v2";

	
	

	// spawns a shapenet actor group from json, recursively spawns all children
	void importShapenetActorGroup(json::object_t actorGroup, FVector origin);

	// samples a location from a json location object
	float sampleLocation(json::object_t &location);


	// spawns a single actor
	void importShapenetActor(json::object_t actor, FVector origin);

	void listDescendants(json::object_t&  actorGroup);

	// modifies json to pass down actorParams from parents to children
	void passDownParams(json::object_t &srcObj);
	
	// helper function for transfer parameters between two actorParams json objects
	void transferParamsBetween(json::object_t &srcObj, json::object_t &dstObj);

	FDateTime roomJsonLastChanged;
	

	// resets physics damping to default values
	void resetDamping();


	bool objectsDamped;

	json parsed;


	FDateTime roomJsonTimeStamp = FDateTime(0);

	void spawnLighting();

	FTimerHandle FuzeTimerHandle;

	// automatically spawns floor also spawns ceiling and sidewalls if parameters set to true, assumes rectangular walls
	void autoSpawnWalls(bool autoSpawnSideWalls=false, bool autoSpawnCeiling=false);


	// utility function for parsing json object file
	json::object_t parseJsonFromPath(FString path);
	
	//utility function for parsing json array file
	json::array_t parseJsonArrayFromPath(FString path);

	// agent interacts with object
	virtual bool interact(AGymAgent* agent, AGymObj* obj);

	// object interacts with agent (different from reverse to allow for asymmetrial interaction
	virtual bool interact(AGymObj* obj, AGymAgent* agent);

	// interaction bewteen two agents
	virtual bool interact(AGymAgent* agent1, AGymAgent* agent2);

	// interaction between two objects
	virtual bool interact(AGymObj* obj1, AGymObj* obj2);

	// returns gym objects in game
	TArray<AGymObj*> getGymObjects();
	
	// returns gym objects with interaction enabled
	TArray<AGymObj*> getInteractiveGymObjects();
	TArray<AGymObj*> getAgentInteractiveGymObjects();
	TArray<AGymObj*> getObjectInteractiveGymObjects();
	
	// returns gym agents in game
	TArray<AGymAgent*> getGymAgents();

	// gets reward for current time step for agent
	void distributeReward(AGymAgent* agent);

	FString getMode();
	void setMode(FString newMode);

	void toggleFire();
	void dispenseSalt();

	void attachToAgent(class AGDemoAgent* agent);

	UFUNCTION()
	void asyncAttach(AGDemoAgent* agent, AGymObj* closest, int index);

	void cook();

	void disablePan();

	void toggleLighter();
	
	void crack();

private:
	// objects in game
	TArray<AGymObj*> gymObjects;

	// agents in game
	TArray<AGymAgent*> gymAgents;

	// primary agent, automatically possesd by player
	AGymAgent* primaryAgent;


	int numResets;

	FString mode = "indoor";

	int32 crackMode = 0;

	

	AGymObj* meat;
	AGymObj* heatSource;
	AGymObj* salt;
	AGymObj* lighter;
	AGymObj* wood;
	AGymObj* pan;
	AGymObj* knife;

	AGymObj* walnut;
	AGymObj* hammer;
	AGymObj* brick;



	// other hammer objs
	AGymObj* wrench;
	AGymObj* waterBottle;

	// other brick objs
	AGymObj* banana;
	AGymObj* candyBar;
	AGymObj* cereal;

	AGymObj* stone;
	AGymObj* book;
	AGymObj* grater;

};
