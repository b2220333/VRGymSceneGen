// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shapenet.generated.h"


USTRUCT()
struct FShapenetObject
{
	GENERATED_BODY()


	UPROPERTY()
	FString synset;

	UPROPERTY()
	FString hash;


};

USTRUCT()
struct FImportJson
{
	GENERATED_BODY()


	UPROPERTY()
	TArray<FString> searchTerms;

	UPROPERTY()
	TArray<FString> synsets;

	UPROPERTY()
	TArray<FShapenetObject> shapenetObjects;

	UPROPERTY()
	TArray<FString> modelPath;


};

USTRUCT()
struct FSynsetChildren
{
	GENERATED_BODY()


	UPROPERTY()
	TArray<FString> children;

	
};


USTRUCT()
struct FSynsetObj
{
	GENERATED_BODY()


	UPROPERTY()
	FString synsetId;

	UPROPERTY()
	FString name;

	/*
	UPROPERTY()
	FSynsetChildren children;
	*/

	UPROPERTY()
	int32 numInstances;
};

/*
USTRUCT()
struct FTaxonomy
{
	GENERATED_BODY()


	UPROPERTY()
	TArray<FSynsetObj> synsets;


};
*/




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

	bool importMesh(FString synset, FString hash);
	
	UStaticMeshComponent* BaseMesh;
	
};
