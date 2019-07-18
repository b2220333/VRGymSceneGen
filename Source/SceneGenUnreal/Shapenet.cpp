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


// Sets default values
AShapenet::AShapenet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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


void AShapenet::importMesh(FString synset, FString hash)
{
	FString path = "/Game/ShapenetObj/" + synset + "/" + hash + "/model_normalized.model_normalized";
	importMeshFromFile(path);
}

void AShapenet::importMeshFromFile(FString path)
{
	
	
	UStaticMesh* staticMeshReference = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *path));
	
	BaseMesh = NewObject<UStaticMeshComponent>(this, "BaseMesh");
	RootComponent = BaseMesh;
	BaseMesh->SetStaticMesh(staticMeshReference);
	BaseMesh->SetMobility(EComponentMobility::Movable);
	BaseMesh->RegisterComponent();
	
}

void AShapenet::importRandomFromSynset(FString synset)
{
	IFileManager& FileManager = IFileManager::Get();
	FString path = FPaths::ProjectContentDir() + synset + "/*.*";
	UE_LOG(LogTemp, Warning, TEXT("Importing random mesh from %s"), *path);
	TArray<FString> Hashes;
	FileManager.FindFiles(Hashes, *path, false, true);
	UE_LOG(LogTemp, Warning, TEXT("Found %d models"), Hashes.Num());
	int32 i = FMath::RandRange(0, Hashes.Num() - 1);
	if (Hashes.Num() > 0) {
		importMesh(synset, Hashes[i]);
	}
}
