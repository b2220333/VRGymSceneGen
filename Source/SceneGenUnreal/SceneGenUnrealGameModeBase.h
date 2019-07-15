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



};

USTRUCT()
struct FRoomDims
{
	GENERATED_BODY()

	UPROPERTY()
	float xWidth;

	UPROPERTY()
	float yWidth;

	UPROPERTY()
	float zWidth;


};

USTRUCT()
struct FRoomJson
{
	GENERATED_BODY()

	UPROPERTY()
	FString name;

	UPROPERTY()
	FRoomDims roomDimensions;

	UPROPERTY()
	TArray<FShapenetActor> shapnetObjects;

	


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

	
};
