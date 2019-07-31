// Fill out your copyright notice in the Description page of Project Settings.

#include "Shapenet.h"
/*
#include "fbxsdk.h"

#include "FbxImporter.h"

*/

#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"

#include "Runtime/Core/Public/GenericPlatform/GenericPlatformFile.h"
#include "Runtime/Core/Public/HAL/FileManager.h"
#include "Editor/UnrealEd/Public/FileHelpers.h"

#include "SceneGenUnrealGameModeBase.h"

#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "PhysicsEngine/BodySetup.h"
#include "Runtime/Engine/Classes/PhysicalMaterials/PhysicalMaterial.h"


// Sets default values
AShapenet::AShapenet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShapenet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShapenet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}






UMaterialInterface* AShapenet::getRandomMaterial()
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
	FString subPath = assetFiles[i].Mid(index + 21, assetFiles[i].Len() - (index+21));

	FString matPath = "/Game" + FPaths::GetBaseFilename(subPath, false) + "." + FPaths::GetBaseFilename(subPath, true);
	return Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, *matPath));
}


void AShapenet::importRandomFromSynset(FString synset, FVector location, json::object_t params)
{
	IFileManager& FileManager = IFileManager::Get();
	FString path = FPaths::ProjectContentDir() + "shapenetOBJ/" + synset + "/*.*";
	UE_LOG(LogTemp, Warning, TEXT("Importing random mesh from %s"), *path);
	TArray<FString> Hashes;
	FileManager.FindFiles(Hashes, *path, false, true);
	UE_LOG(LogTemp, Warning, TEXT("Found %d models"), Hashes.Num());
	int32 i = FMath::RandRange(0, Hashes.Num() - 1);
	if (Hashes.Num() > 0) {
		importMeshFromSynsetAndHash(synset, Hashes[i], location, params);
	}
}

void AShapenet::importMeshFromFile(FString path, FVector location, json::object_t params)
{
	if (params["spawnProbability"].is_number()) {
		float rng = FMath::RandRange(0.0f, 1.0f);
		if (rng > params["spawnProbability"] && params["spawnProbability"] >= 0.0f) {
			return;
		}
	}

	

	UStaticMesh* staticMeshReference = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *path));
	if (staticMeshReference) {
		modelPath = path;
		BaseMesh = NewObject<UStaticMeshComponent>(this, "BaseMesh");
		BaseMesh->SetMobility(EComponentMobility::Movable);
		RootComponent = BaseMesh;
		RootComponent->SetMobility(EComponentMobility::Movable);
		BaseMesh->SetStaticMesh(staticMeshReference);

		//if (params["useRandomTextures"].is_boolean() && params["useRandomTextures"]) {
		int32 numMats = BaseMesh->GetNumMaterials();
		for (int32 i = 0; i < numMats; i++) {
			UMaterialInterface* material = getRandomMaterial();
			BaseMesh->SetMaterial(i, material);
		}
		//}

		// physics comes last to allow for other setup first
		if (params["physicsEnabled"].is_boolean() && !params["physicsEnabled"].get<bool>()) {
			BaseMesh->SetSimulatePhysics(false);
			BaseMesh->SetEnableGravity(false);
		}
		else {
			BaseMesh->SetSimulatePhysics(true);
			BaseMesh->SetEnableGravity(true);
			BaseMesh->SetLinearDamping(20);

			//UPhysicalMaterial* physMat = BaseMesh->GetBodySetup()->GetPhysMaterial();
			//physMat->Density = 10;
		}


		FBox box = BaseMesh->Bounds.GetBox();
		FVector extents = box.GetExtent();
		location.Z = extents.Z + 1;
		originalSpawnLocation = location;
		RootComponent->SetWorldLocation(location);


		/*
		FVector offset = FVector(0, 0, -extents.Z);
		FVector currCoM = BaseMesh->GetCenterOfMass();
		BaseMesh->SetCenterOfMass(currCoM + offset);
		*/



		BaseMesh->RegisterComponent();
	}
}

void AShapenet::importMeshFromSynsetAndHash(FString synset, FString hash, FVector location, json::object_t params)
{
	FString path = "/Game/ShapenetObj/" + synset + "/" + hash + "/model_normalized.model_normalized";
	importMeshFromFile(path, location, params);
}

void AShapenet::spawnFloor(float x, float y)
{
	// spawn floor with x width, y length centered at (0,0,0)
	UStaticMesh* staticMeshReference = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/DefaultFloor.DefaultFloor")));
	BaseMesh = NewObject<UStaticMeshComponent>(this, "BaseMesh");
	BaseMesh->SetMobility(EComponentMobility::Movable);
	RootComponent = BaseMesh;
	RootComponent->SetWorldLocation(FVector(0,0,0));
	RootComponent->SetWorldRotation(FRotator(-90, 0, 0));
	RootComponent->SetWorldScale3D(FVector(1, x / 256, y / 256));
	RootComponent->SetMobility(EComponentMobility::Movable);
	SetActorLabel("SpawnedFloor");
	BaseMesh->SetStaticMesh(staticMeshReference);
	BaseMesh->RegisterComponent();

	
}


UStaticMeshComponent* AShapenet::getBaseMesh() {
	return BaseMesh;
}

bool AShapenet::importNew(FVector location, json::object_t params)
{

	if (params["meshOverride"].is_string()) {
		FString meshOverride = FString(params["meshOverride"].get<json::string_t>().c_str());
		importMeshFromFile(meshOverride, location, params);
	}
	else if (params["shapenetSynset"].is_string() && params["shapenetHash"].is_string()) {
		FString synset = FString(params["shapenetSynset"].get<json::string_t>().c_str());
		FString hash = FString(params["shapenetHash"].get<json::string_t>().c_str());
		importMeshFromSynsetAndHash(synset, hash, location, params);
	}
	else if (params["shapenetSynset"].is_string()) {
		FString synset = FString(params["shapenetSynset"].get<json::string_t>().c_str());
		importRandomFromSynset(synset, location, params);
	}


	return true;
}



bool AShapenet::tryRespawnNewCM()
{
	return false; // do nothing for now
	bool stable = true;
	
	if (FMath::Abs(GetActorRotation().Roll) > 10 || FMath::Abs(GetActorRotation().Pitch) > 10) {
		// redo CoM
		if (BaseMesh) {
			//RootComponent->SetWorldLocation(originalSpawnLocation);
			//SetActorRotation(FRotator::ZeroRotator);

			// reset rotation
			
			/*
			while (FMath::Abs(GetActorRotation().Roll) > 1 || FMath::Abs(GetActorRotation().Pitch) > 1) {
				if (FMath::Abs(GetActorRotation().Roll) > 1) {
					BaseMesh->AddTorqueInDegrees(FVector(GetActorRotation().Roll,0,0));
				}
				if (FMath::Abs(GetActorRotation().Pitch) > 1) {
					BaseMesh->AddTorqueInDegrees(FVector(0, GetActorRotation().Pitch, 0));
				}
			}
			
			

			
			FVector offset = FVector(0, 0, 0);
			offset.Y = (GetActorRotation().Roll / 10) * -1;
			offset.X = (GetActorRotation().Pitch / 10) * -1;
			

			UE_LOG(LogTemp, Warning, TEXT("Set new CoM offset by: (%f, %f, %f)"), offset.X, offset.Y, offset.Z);
			
			FVector currCoM = BaseMesh->GetCenterOfMass();
			BaseMesh->SetCenterOfMass(currCoM + offset);
			*/

			//BaseMesh->SetCenterOfMass(currCoM + FVector(0,-20,0));

			BaseMesh->GetBodyInstance()->UpdateMassProperties();
			
			return true;
		}
	}

	return false;
	
}