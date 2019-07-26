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


// Sets default values
AShapenet::AShapenet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//importMesh(synset, hash);

	//importMesh("02818832", "2f44a88e17474295e66f707221b74f43");

	/*
	FString synset = "04554684";
	FString hash = "fcc0bdba1a95be2546cde67a6a1ea328";

	importMesh(synset, hash);
	*/

	/*
		testing import FBX third party library

	// create a SdkManager
	FbxManager *lSdkManager = FbxManager::Create();

	// create an IOSettings object
	FbxIOSettings * ios = FbxIOSettings::Create(lSdkManager, IOSROOT);

	// set some IOSettings options 
	ios->SetBoolProp(IMP_FBX_MATERIAL, true);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);

	// create an empty scene
	FbxScene* lScene = FbxScene::Create(lSdkManager, "");

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	// Initialize the importer by providing a filename and the IOSettings to use
	lImporter->Initialize("C:\\myfile.fbx", -1, ios);

	// Import the scene.
	lImporter->Import(lScene);

	// Destroy the importer.
	lImporter->Destroy();


	*/
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


void AShapenet::importRandomFromSynset(FString synset, FVector location, json::object_t param)
{
	IFileManager& FileManager = IFileManager::Get();
	FString path = FPaths::ProjectContentDir() + "shapenetOBJ/" + synset + "/*.*";
	UE_LOG(LogTemp, Warning, TEXT("Importing random mesh from %s"), *path);
	TArray<FString> Hashes;
	FileManager.FindFiles(Hashes, *path, false, true);
	UE_LOG(LogTemp, Warning, TEXT("Found %d models"), Hashes.Num());
	int32 i = FMath::RandRange(0, Hashes.Num() - 1);
	if (Hashes.Num() > 0) {
		importMeshFromSynsetAndHash(synset, Hashes[i], location, param);
	}
}

void AShapenet::importMeshFromFile(FString path, FVector location, json::object_t param)
{
	if (param["spawnProbability"].is_number()) {
		float rng = FMath::RandRange(0.0f, 1.0f);
		if (rng > param["spawnProbability"] && param["spawnProbability"] >= 0.0f) {
			return;
		}
	}

	

	UStaticMesh* staticMeshReference = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *path));
	BaseMesh = NewObject<UStaticMeshComponent>(this, "BaseMesh");
	BaseMesh->SetMobility(EComponentMobility::Movable);
	RootComponent = BaseMesh;
	RootComponent->SetWorldLocation(location);
	RootComponent->SetMobility(EComponentMobility::Movable);
	BaseMesh->SetStaticMesh(staticMeshReference);

	//if (param["useRandomTextures"].is_boolean() && param["useRandomTextures"]) {
		int32 numMats = BaseMesh->GetNumMaterials();
		for (int32 i = 0; i < numMats; i++) {
			UMaterialInterface* material = getRandomMaterial();
			BaseMesh->SetMaterial(i, material);
		}
	//}

	// physics comes last to allow for other setup first
	if ( (param["physicsEnabled"].is_boolean() && !param["physicsEnabled"].get<bool>()) ) {
		BaseMesh->SetSimulatePhysics(false);
		BaseMesh->SetEnableGravity(false);
	} else {
		BaseMesh->SetSimulatePhysics(true);
		BaseMesh->SetEnableGravity(true);
	}


	FBox box = BaseMesh->Bounds.GetBox();
	FVector extents = box.GetExtent();
	location.Z = extents.Z;
	RootComponent->SetWorldLocation(location);
	BaseMesh->RegisterComponent();
}

void AShapenet::importMeshFromSynsetAndHash(FString synset, FString hash, FVector location, json::object_t param)
{
	FString path = "/Game/ShapenetObj/" + synset + "/" + hash + "/model_normalized.model_normalized";
	importMeshFromFile(path, location, param);
}

void AShapenet::spawnFloor(float x, float y)
{
	// spawn floor with x width, y length centered at (0,0,0)

	/*
	UBoxComponent* boxComponent = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), TEXT("FloorBoxComponent"));
	RootComponent = boxComponent;
	FVector extent = FVector(x / 2, y / 2, 0.01);
	boxComponent->SetBoxExtent(extent);

	UStaticMeshComponent* floorVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorVisual"));
	floorVisual->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeVisualAsset(TEXT("/Engine/EngineMeshes/Cube.Cube"));
	if (CubeVisualAsset.Succeeded())
	{
		floorVisual->SetStaticMesh(CubeVisualAsset.Object);
		floorVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		floorVisual->SetWorldScale3D(FVector(1.0f));
	}
	*/

	UStaticMesh* staticMeshReference = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/DefaultFloor.DefaultFloor")));
	BaseMesh = NewObject<UStaticMeshComponent>(this, "BaseMesh");
	BaseMesh->SetMobility(EComponentMobility::Movable);
	RootComponent = BaseMesh;
	RootComponent->SetWorldLocation(FVector(0,0,0));
	RootComponent->SetWorldRotation(FRotator(-90, 0, 0));
	RootComponent->SetWorldScale3D(FVector(1, x / 256, y / 256));
	RootComponent->SetMobility(EComponentMobility::Movable);
	
	BaseMesh->SetStaticMesh(staticMeshReference);


	BaseMesh->RegisterComponent();

	
}