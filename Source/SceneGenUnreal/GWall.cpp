// Fill out your copyright notice in the Description page of Project Settings.

#include "GWall.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"

bool AGWall::spawnWall(float xWidth, float yWidth, float zWidth, FTransform wallTransform)
{

	// spawn floor with x width, y length centered at (0,0,0)
	UStaticMesh* staticMeshReference = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/DefaultFloor.DefaultFloor")));
	UStaticMeshComponent* baseMeshReference = getBaseMesh();
	if (staticMeshReference && baseMeshReference) {
		baseMeshReference = NewObject<UStaticMeshComponent>(this, "BaseMesh");
		baseMeshReference->SetMobility(EComponentMobility::Movable);
		RootComponent = baseMeshReference;
		RootComponent->SetWorldLocation(FVector(0, 0, 0));
		RootComponent->SetWorldRotation(FRotator(-90, 0, 0));
		RootComponent->SetWorldScale3D(FVector(1, xWidth / 256, yWidth / 256));
		RootComponent->SetMobility(EComponentMobility::Movable);
		SetActorLabel("SpawnedFloor");
		baseMeshReference->SetStaticMesh(staticMeshReference);
		baseMeshReference->RegisterComponent();

		return true;
	}
	return false;
}

bool AGWall::spawnFloor(float xWidth, float yWidth)
{
	FTransform wallTransform;

	return spawnWall(xWidth, yWidth, 0, wallTransform);
}

