// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shapenet.generated.h"


USTRUCT()
struct FSearchTerms
{
	GENERATED_BODY()


	UPROPERTY()
	FString query;

	UPROPERTY()
	int32 numModelsToImport = -1;


};

USTRUCT()
struct FImportSynsets
{
	GENERATED_BODY()


	UPROPERTY()
	FString synset;

	UPROPERTY()
	int32 numModelsToImport = -1;


};


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
struct FModelManual
{
	GENERATED_BODY()


	UPROPERTY()
	FString srcPath;

	UPROPERTY()
	FString dstPath;


};


USTRUCT()
struct FImportJson
{
	GENERATED_BODY()


	UPROPERTY()
	TArray<FSearchTerms> searchTerms;

	UPROPERTY()
	TArray<FImportSynsets> synsets;

	UPROPERTY()
	TArray<FShapenetObject> shapenetObjects;

	UPROPERTY()
	TArray<FModelManual> modelsManual;


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

	void importMesh(FString synset, FString hash, FVector location);

	void importRandomFromSynset(FString synset, FVector location);

	void importMeshFromFile(FString path, FVector location);
	
	UStaticMeshComponent* BaseMesh;
	
	FString synset;
	FString hash;
};
