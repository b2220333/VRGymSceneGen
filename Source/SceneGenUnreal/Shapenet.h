// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "json.hpp"
using json = nlohmann::json;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shapenet.generated.h"




struct FActorParams;

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

	void importMeshFromSynsetAndHash(FString synset, FString hash, FVector location, json::object_t param);
	void importRandomFromSynset(FString synset, FVector location, json::object_t param);
	void importMeshFromFile(FString path, FVector location, json::object_t param);


	

	UMaterialInterface* getRandomMaterial();

	UMaterialInterface* getRandomMaterialFromDir(FString path);

	UStaticMeshComponent* BaseMesh;
	
	FString synset;
	FString hash;
};
