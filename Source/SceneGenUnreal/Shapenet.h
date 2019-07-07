// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shapenet.generated.h"


USTRUCT()
struct FTaxonomyStruct
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere)
	FString synsetId;

	UPROPERTY(EditAnywhere)
	FString name;

	UPROPERTY(EditAnywhere)
	FString children;

	UPROPERTY(EditAnywhere)
	int32 numInstances;
};


/*
USTRUCT()
struct FTest
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere)
		FString test;
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
