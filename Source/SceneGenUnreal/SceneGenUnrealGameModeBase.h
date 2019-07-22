// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Shapenet.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SceneGenUnrealGameModeBase.generated.h"


USTRUCT()
struct FActorParams
{
	GENERATED_BODY()


	UPROPERTY()
	FString shapenetSynset = "";

	UPROPERTY()
	FString shapenetHash = "";

	UPROPERTY()
	FString meshFromOverride = "";

	UPROPERTY()
	FString textureOverride = "";

	UPROPERTY()
	int32 quantity = -1;

	UPROPERTY()
	float spawnProbability = -1.0f;

	UPROPERTY()
	int32 destructable = -1;

	UPROPERTY()
	int32 physicsEnabled = -1;

	UPROPERTY()
	int32 useRandomTextures = -1;

	UPROPERTY()
	int32 useSameMeshForAllInstances = -1;

	UPROPERTY()
	int32 useSameTextureForAllInstances = -1;

	UPROPERTY()
	int32 canOverlap = -1;

};

USTRUCT()
struct FShapenetActor
{
	GENERATED_BODY()

	UPROPERTY()
	FString name;
	
	UPROPERTY()
	float x;

	UPROPERTY()
	float y;

	UPROPERTY()
	float z;

	// override actor group properties

	UPROPERTY()
	FActorParams actorParams;

};

USTRUCT()
struct FShapenetActorGroup
{

	GENERATED_BODY()

	UPROPERTY()
	FString name;

	
	UPROPERTY()
	float xCenter;

	UPROPERTY()
	float yCenter;

	UPROPERTY()
	float zCenter;


	UPROPERTY()
	FActorParams actorParams;

	UPROPERTY()
	TArray<FShapenetActor> shapenetActors;

	

	UPROPERTY()
	TArray<FString> childShapenetActorGroups;

	
	// internal use (not in json)
	FShapenetActorGroup* parentGroup;
	TArray<FShapenetActorGroup*> childGroups;
	FVector groupOrigin = FVector(0.0f, 0.0f, 0.0f);
	
};

USTRUCT()
struct FRoomJson
{
	GENERATED_BODY()

	UPROPERTY()
	FString name;

	UPROPERTY()
	float xWidth;

	UPROPERTY()
	float yWidth;

	UPROPERTY()
	float zWidth;

	UPROPERTY()
	TArray<FShapenetActorGroup> shapenetActorGroups;


};

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

	TArray<AShapenet*> shapenetActors;
	

	void randomizeTextures(AShapenet shapenetActor);

	void randomizePosition(AShapenet shapenetActor, int32 constraints);

	void importShapenetActorGroup(FShapenetActorGroup* actorGroup, FActorParams* params);

	void importShapenetActor(FShapenetActor* actor, FActorParams* params, FVector* origin);

	void transferParams(FActorParams* parentParams, FActorParams* childParams);

	TArray<FShapenetActorGroup*> linkShapenetActorGroups(TArray<FShapenetActorGroup>* actorGroups);

	void listDescendants(FShapenetActorGroup* actorGroup);

};
