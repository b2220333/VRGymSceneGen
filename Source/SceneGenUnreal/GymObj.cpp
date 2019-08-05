// Fill out your copyright notice in the Description page of Project Settings.

#include "GymObj.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/Core/Public/HAL/FileManager.h"

// Sets default values
AGymObj::AGymObj()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGymObj::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGymObj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UStaticMeshComponent* AGymObj::getbaseMesh()
{
	return baseMesh;
}



bool AGymObj::importMeshFromPath(FString path, FVector location, json::object_t params)
{
	if (params["spawnProbability"].is_number()) {
		float rng = FMath::RandRange(0.0f, 1.0f);
		if (rng > params["spawnProbability"] && params["spawnProbability"] >= 0.0f) {
			return false;
		}
	}



	UStaticMesh* staticMeshReference = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *path));
	if (!staticMeshReference) {
		return false;
	}
	baseMesh = NewObject<UStaticMeshComponent>(this, "baseMesh");
	baseMesh->SetMobility(EComponentMobility::Movable);
	RootComponent = baseMesh;
	RootComponent->SetMobility(EComponentMobility::Movable);
	baseMesh->SetStaticMesh(staticMeshReference);

	//if (params["useRandomTextures"].is_boolean() && params["useRandomTextures"]) {
	int32 numMats = baseMesh->GetNumMaterials();
	for (int32 i = 0; i < numMats; i++) {
		UMaterialInterface* material = getRandomMaterial();
		baseMesh->SetMaterial(i, material);
	}
	//}

	// physics comes last to allow for other setup first
	if (params["physicsEnabled"].is_boolean() && !params["physicsEnabled"].get<bool>()) {
		baseMesh->SetSimulatePhysics(false);
		baseMesh->SetEnableGravity(false);
	}
	else {
		baseMesh->SetSimulatePhysics(true);
		baseMesh->SetEnableGravity(true);
		baseMesh->SetLinearDamping(20);

		//UPhysicalMaterial* physMat = baseMesh->GetBodySetup()->GetPhysMaterial();
		//physMat->Density = 10;
	}


	FBox box = baseMesh->Bounds.GetBox();
	FVector extents = box.GetExtent();
	location.Z = extents.Z + 1;
	originalSpawnLocation = location;
	RootComponent->SetWorldLocation(location);


	/*
	FVector offset = FVector(0, 0, -extents.Z);
	FVector currCoM = baseMesh->GetCenterOfMass();
	baseMesh->SetCenterOfMass(currCoM + offset);
	*/

	baseMeshPath = path;
	importParams = params;
	baseMesh->RegisterComponent();
	return true;
}

bool AGymObj::importMeshesFromPath(FString path, FVector location, json::object_t params)
{
	if (!baseMesh) {
		UE_LOG(LogTemp, Warning, TEXT("GymObj.importMeshesFromPath: must first set baseMesh"))
		return false;
	}



	return true;
}

json::object_t AGymObj::getImportParams()
{
	return importParams;
}

UMaterialInterface* AGymObj::getRandomMaterial()
{
	IFileManager& FileManager = IFileManager::Get();
	TArray<FString> assetFiles;
	FString fileName = "*.uasset";
	FString path = FPaths::ProjectContentDir() + "shapenetOBJ/";
	FileManager.FindFilesRecursive(assetFiles, *path, *fileName, true, false, false);

	bool found = false;

	int32 i;
	while (!found) {
		i = FMath::RandRange(0, assetFiles.Num() - 1);
		if (assetFiles[i].Contains("material")) {
			found = true;
		}
	}

	int32 index = assetFiles[i].Find(TEXT("VRGymSceneGen/Content/"));
	FString subPath = assetFiles[i].Mid(index + 21, assetFiles[i].Len() - (index + 21));

	FString matPath = "/Game" + FPaths::GetBaseFilename(subPath, false) + "." + FPaths::GetBaseFilename(subPath, true);
	return Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, *matPath));
}