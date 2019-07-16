// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Shapenet.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SceneGenUnrealGameModeBase.generated.h"

USTRUCT()
struct FShapenetActor
{
	GENERATED_BODY()

	UPROPERTY()
	FString name;


	// override actor group properties

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

};

USTRUCT()
struct FShapenetActorGroup
{

	GENERATED_BODY()

	UPROPERTY()
	FString name;

	UPROPERTY()
	int32 xCenter;

	UPROPERTY()
	int32 yCenter;

	UPROPERTY()
	int32 zCenter;


	// same properties as actors

	UPROPERTY()
	TArray<FShapenetActor> shapnetActors;
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
	TArray<FShapenetActorGroup> shapnetActorGroups;


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
	void spawnShapnets();

	TArray<AShapenet> shapenetActors;
	

	void randomizeTextures(AShapenet shapenetActor);

	void randomizePosition(AShapenet shapenetActor, int32 constraints);

	
};
