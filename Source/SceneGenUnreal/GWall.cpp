// Fill out your copyright notice in the Description page of Project Settings.

#include "GWall.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"

bool AGWall::spawnWall(FTransform wallTransform)
{
	UStaticMesh* staticMeshReference = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/DefaultFloor.DefaultFloor")));
	if (staticMeshReference) {
		setBaseMesh(NewObject<UStaticMeshComponent>(this, "BaseMesh"));
		getBaseMesh()->SetMobility(EComponentMobility::Movable);
		RootComponent = getBaseMesh();
		RootComponent->SetWorldTransform(wallTransform);
		RootComponent->SetMobility(EComponentMobility::Movable);
		getBaseMesh()->SetStaticMesh(staticMeshReference);
		getBaseMesh()->RegisterComponent();
		return true;
	}
	return false;
}

bool AGWall::spawnFloor(float xWidth, float yWidth)
{
	FTransform wallTransform;
	wallTransform.SetRotation(FQuat(FRotator(-90, 0, 0)));
	wallTransform.SetTranslation(FVector(0, 0, 0));
	wallTransform.SetScale3D(FVector(1, xWidth / 256, yWidth / 256));

	SetActorLabel("SpawnedFloor");
	return spawnWall(wallTransform);
	
}

bool AGWall::applyDemoWallParams()
{
	return setMaterial(woodMaterialPath);
}

