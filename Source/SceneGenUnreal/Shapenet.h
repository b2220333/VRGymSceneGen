// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "json.hpp"
using json = nlohmann::json;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shapenet.generated.h"





UCLASS()
class SCENEGENUNREAL_API AShapenet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShapenet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	bool importNew(FVector location, json::object_t params);
	void importMeshFromSynsetAndHash(FString synset, FString hash, FVector location, json::object_t params);
	void importRandomFromSynset(FString synset, FVector location, json::object_t params);
	void importMeshFromFile(FString path, FVector location, json::object_t params);

	void importPartnet(FString annotationID);
	FString partnetDir = "D://data/data_v0"; //clone partnet github repo into this directory


	void spawnFloor(float x, float y);

	UMaterialInterface* getRandomMaterial();

	UMaterialInterface* getRandomMaterialFromDir(FString path);

	
	UStaticMeshComponent* getBaseMesh();
	
	FString synset;
	FString hash;
	FString modelPath;
	FVector originalSpawnLocation;

	bool tryRespawnNewCM();

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BaseMesh;
};
