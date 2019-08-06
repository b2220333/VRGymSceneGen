// Fill out your copyright notice in the Description page of Project Settings.

#include "GymObj.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/Core/Public/HAL/FileManager.h"
#include "Runtime/AssetRegistry/Public/ARFilter.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsConstraintComponent.h"
#include "Runtime/Engine/Classes/PhysicsEngine/ConstraintInstance.h"

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

UStaticMeshComponent* AGymObj::getBaseMesh()
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

	applyParamsToMesh(baseMesh, params);

	// set private memebers after successful setup
	baseMeshPath = path;
	importParams = params;
	baseMesh->RegisterComponent();
	return true;
}

bool AGymObj::importMeshesFromPath(FString path, FVector location, json::object_t params)
{
	if (!baseMesh || !RootComponent) {
		UE_LOG(LogTemp, Warning, TEXT("GymObj.importMeshesFromPath: must first set baseMesh"))
		return false;
	}

	TArray<UStaticMesh*> staticMeshes;
	getAssetsOfClass<UStaticMesh>(staticMeshes, { path }, true, true);

	for (int32 i = 0; i < staticMeshes.Num(); i++) {
		if (staticMeshes[i]->GetName() != baseMesh->GetStaticMesh()->GetName()) {
			UStaticMeshComponent* child = NewObject<UStaticMeshComponent>(this, FName(*staticMeshes[i]->GetName()));
			child->SetStaticMesh(staticMeshes[i]);
			
			
			
			// welding automatically attaches child to baseMesh
			child->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));
			bool welded = child->WeldToImplementation(baseMesh);
			child->RegisterComponent();
			//applyParamsToMesh(child, params);
			
			/*
			FString physicsConstraintName = "Constraint";
			physicsConstraintName.AppendInt(i);
			UPhysicsConstraintComponent* constraint = NewObject<UPhysicsConstraintComponent>(this, *physicsConstraintName);
			constraint->SetupAttachment(RootComponent);

			constraint->ConstraintActor1 = this;
			constraint->ConstraintActor2 = this;

			// FConstraintInstance constraintInstance;
			// constraint->ConstraintInstance = constraintInstance;

			constraint->SetConstrainedComponents(baseMesh, baseMesh->GetFName(), child, child->GetFName());
			constraint->SetConstraintReferencePosition(EConstraintFrame::Type::Frame1, FVector::ZeroVector);

			
			constraint->SetWorldLocation(RootComponent->GetComponentLocation());
			constraint->RegisterComponent();
			*/
			

			/*
			FTransform relativeTransform = FTransform(FVector::ZeroVector);
			bool welded = baseMesh->BodyInstance.Weld(&child->BodyInstance, relativeTransform);
			
			if (child->BodyInstance.BodySetup.IsValid()) {
				UE_LOG(LogTemp, Warning, TEXT("Setup Valid"))
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Setup invalid"))
			}
			*/

			if (welded) {
				UE_LOG(LogTemp, Warning, TEXT("Successful Weld"))
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Failed to Weld"))
			}
			
			
			additionalMeshes.Add(child);
			
			
		}
	}

	locationSetup(location, params);
	return true;
}

json::object_t AGymObj::getImportParams()
{
	return importParams;
}

UMaterialInterface* AGymObj::getRandomMaterialFrom(TArray<FString> paths,  bool recursivePaths)
{

	TArray<UMaterialInterface*> materialAssets;
	getAssetsOfClass<UMaterialInterface>(materialAssets, paths, true, recursivePaths);

	if (materialAssets.Num() > 0) {
		int32 i = FMath::RandRange(0, materialAssets.Num() - 1);
		return materialAssets[i];
	}
	return nullptr;
}

template<typename T>
static void AGymObj::getAssetsOfClass(TArray<T*>& OutArray, TArray<FString> paths, bool recursiveClasses, bool recursivePaths)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;
	FARFilter Filter;
	Filter.ClassNames.Add(T::StaticClass()->GetFName());
	Filter.bRecursivePaths = recursivePaths;
	Filter.bRecursiveClasses = recursiveClasses;
	
	for (int32 i = 0; i < paths.Num(); i++) {
		Filter.PackagePaths.Add(FName(*paths[i]));
	}
	
	AssetRegistryModule.Get().GetAssets(Filter , AssetData);
	for (int32 i = 0; i < AssetData.Num(); i++) {
		T* Object = Cast<T>(AssetData[i].GetAsset());
		OutArray.Add(Object);
	}

	UE_LOG(LogTemp, Warning, TEXT("AGymObj.getAssetsOfClass: Found %d instances of %s"), OutArray.Num(), *T::StaticClass()->GetFName().ToString())
}

void AGymObj::applyParamsToMesh(UStaticMeshComponent* mesh, json::object_t params)
{
	//if (params["useRandomTextures"].is_boolean() && params["useRandomTextures"]) {
	int32 numMats = mesh->GetNumMaterials();
	for (int32 i = 0; i < numMats; i++) {
		UMaterialInterface* material = getRandomMaterialFrom({ "/Game" }, true);
		mesh->SetMaterial(i, material);
	}
	//}

	
	// physics comes last to allow for other setup first
	if (params["physicsEnabled"].is_boolean() && !params["physicsEnabled"].get<bool>()) {
		mesh->SetSimulatePhysics(false);
		mesh->SetEnableGravity(false);
	}
	else {
		mesh->SetSimulatePhysics(true);
		mesh->SetEnableGravity(true);
		// damping for stable setup, to be removed after all actors have spawned and settled in position
		mesh->SetLinearDamping(20);

		//UPhysicalMaterial* physMat = baseMesh->GetBodySetup()->GetPhysMaterial();
		//physMat->Density = 10;
	}
	

}

void AGymObj::locationSetup(FVector location, json::object_t params)
{
	// spawn object directly above floor
	FVector origin;
	FVector extents;
	GetActorBounds(false, origin, extents);

	location.Z = extents.Z + 1;
	originalSpawnLocation = location;
	RootComponent->SetWorldLocation(location);
}