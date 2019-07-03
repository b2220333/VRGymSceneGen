// Fill out your copyright notice in the Description page of Project Settings.

#include "Shapenet.h"
#include "fbxsdk.h"
#include <string>
#include "FbxImporter.h"

#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"



// Sets default values
AShapenet::AShapenet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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


bool AShapenet::importMesh(FText synset, FText hash)
{
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BaseMesh"));
	RootComponent = BaseMesh;
	static ConstructorHelpers::FObjectFinder<UStaticMesh>BaseMeshAsset(TEXT("/Game/ShapenetModels/dresser/baseDrawer.baseDrawer"));
	UStaticMesh* Asset = BaseMeshAsset.Object;
	BaseMesh->SetStaticMesh(Asset);
	return false;
}
