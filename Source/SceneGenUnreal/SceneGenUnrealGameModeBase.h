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
	FString shapenetSynset;

	UPROPERTY()
	FString shapenetHash;

	UPROPERTY()
	FString meshFromOverride;

	UPROPERTY()
	FString textureOverride;

	UPROPERTY()
	int32 quantity;

	UPROPERTY()
	float spawnProbability;

	UPROPERTY()
	bool destructable;

	UPROPERTY()
	bool physicsEnabled;

	UPROPERTY()
	bool useRandomTextures;

	UPROPERTY()
	bool useSameMeshForAllInstances;

	UPROPERTY()
	bool useSameTextureForAllInstances;

	UPROPERTY()
	bool canOverlap;

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

	TArray<FShapenetActorGroup*> linkShapenetActorGroups(TArray<FShapenetActorGroup>* actorGroups);

	void listDescendants(FShapenetActorGroup* actorGroup);

};
