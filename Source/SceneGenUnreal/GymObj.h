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

		GymObj (Only for objects not in Shapenet, Partnet, Wall, or Lighting)
		|
		|___GLighting (Unreal pointlights and directional lights)
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

	// gets random material from game content directory path
	UMaterialInterface* getRandomMaterialFrom(TArray<FString> paths,  bool recursivePaths = false);

	// gets all assets of class type T
	template<typename T>
	static void getAssetsOfClass(TArray<T*>& OutArray, TArray<FString> paths = { }, bool recursiveClasses = false, bool recursivePaths = false);

	// applies optionsin parameters json to base mesh only if basemesh already initialized
	void applyParamsToBaseMesh(json::object_t params);

private:
	// name of object
	FString name;
	
	// path to .uasset file in game content folder
	FString baseMeshPath;
	
	// original json parameters used to import
	json::object_t importParams;

	// location where object was originally spawnned
	FVector originalSpawnLocation;
	
	// root component of object all additional meshes are attached to this mesh
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* baseMesh;

	// additional meshes attached to the base mesh
	UPROPERTY(EditAnywhere)
	TArray<UStaticMeshComponent*> additionalMeshes;
};
