// Fill out your copyright notice in the Description page of Project Settings.

#include "ShapenetChair.h"




// Sets default values
AShapenetChair::AShapenetChair()
{
	// unreal engine calls parent class constructor automatically before anything that happens below 

}

// Called when the game starts or when spawned
void AShapenetChair::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AShapenetChair::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShapenetChair::importMeshFromFile(FString path, FVector location, json::object_t params)
{


	return;
}
