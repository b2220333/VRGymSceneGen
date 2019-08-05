// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "json.hpp"
using json = nlohmann::json;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GymObj.generated.h"

UCLASS()
class SCENEGENUNREAL_API AGymObj : public AActor
{
	/*
		Class for objects in VRGym

		GymObj (Only for objects not in Shapenet or Partnet)
		|
		|___GLighting
		|
		|___GWall (Floor, Ceiling, Sidewalls)
		|
		|___GShapenet
		|	|___GSNChair
		|	|___GSNDishwasher
		|	|
		|	.
		|	.
		|	.
		|
		|___GPartnet (No animations / interaction with specific partnet parts)
			|___GPNChair
			|___GPNDishwasher
			|
			.
			.
			.
		|
		.
		.
		.

	*/

	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGymObj();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// assigns base mesh only
	bool importMeshFromPath(FString path, FVector location, json::object_t params);

	// imports all meshes in directory only if base mesh is set first with importMeshFromPath
	bool importMeshesFromPath(FString path, FVector location, json::object_t params);
	
	// gets pointer to gym object's base mesh component
	UStaticMeshComponent* getBaseMesh();

	// get json object of params used to originally import this gym object
	json::object_t getImportParams();

	// gets random material from game content directory
	UMaterialInterface* getRandomMaterial();

private:
	FString name;
	FString baseMeshPath;
	json::object_t importParams;
	FVector originalSpawnLocation;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* baseMesh;
};
