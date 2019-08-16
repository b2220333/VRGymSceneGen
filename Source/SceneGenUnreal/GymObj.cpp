// Fill out your copyright notice in the Description page of Project Settings.

#include "GymObj.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/Core/Public/HAL/FileManager.h"
#include "Runtime/AssetRegistry/Public/ARFilter.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsConstraintComponent.h"
#include "Runtime/Engine/Classes/PhysicsEngine/ConstraintInstance.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystem.h"
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

bool AGymObj::assignMeshFromPath(FString path, FVector location, json::object_t params)
{
	if (params["spawnProbability"].is_number()) {
		float rng = FMath::RandRange(0.0f, 1.0f);
		if (rng > params["spawnProbability"] && params["spawnProbability"] >= 0.0f) {
			return false;
		}
	}

	UStaticMesh* staticMeshReference = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *path));
	if (!staticMeshReference) {
		UE_LOG(LogTemp, Warning, TEXT("Could not find mesh"))
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
	locationSetup(location, params);
	UE_LOG(LogTemp, Warning, TEXT("Mesh assigned"))
	return true;
}

bool AGymObj::assignMeshesFromPath(FString path, FVector location, json::object_t params)
{
	if (!baseMesh || !RootComponent) {
		UE_LOG(LogTemp, Warning, TEXT("GymObj.importMeshesFromPath: must first set baseMesh"))
		return false;
	}

	TArray<UStaticMesh*> staticMeshes;
	getAssetsOfClass<UStaticMesh>(staticMeshes, { path }, true, true);

	for (int32 i = 0; i < staticMeshes.Num(); i++) {
		if (staticMeshes[i]) {
			if (staticMeshes[i]->GetName() != baseMesh->GetStaticMesh()->GetName()) {
				UStaticMeshComponent* child = NewObject<UStaticMeshComponent>(this, FName(*staticMeshes[i]->GetName()));
				child->SetStaticMesh(staticMeshes[i]);
				child->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));

				// merging physics body
				bool welded = child->WeldToImplementation(baseMesh);
				if (welded) {
					UE_LOG(LogTemp, Warning, TEXT("GymObj.importMeshesFromPath: Successful weld"));
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("GymObj.importMeshesFromPath: Failed to weld"));
				}

				child->RegisterComponent();
				json::object_t modifiedParams = params;
				// physics disabled for child meshes when merged for proper functionality
				modifiedParams["physicsEnabled"] = false;
				applyParamsToMesh(child, modifiedParams);
				additionalMeshes.Add(child);
			}
		}
		else {
			return false;
		}
	}
	// setup location of entire actor including base and child meshes
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

bool AGymObj::setMaterial(FString materialAssetPath)
{
	UMaterialInterface* matRef = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), nullptr, *materialAssetPath));
	if (matRef) {
		TArray<UStaticMeshComponent*> meshes;

		meshes.Add(baseMesh);
		meshes.Append(additionalMeshes);
		for (int32 i = 0; i < meshes.Num(); i++) {
			int32 numMats = meshes[i]->GetNumMaterials();
			for (int32 j = 0; j < numMats; j++) {
				meshes[i]->SetMaterial(j, matRef);
			}
		}

		return true;
	}
	return false;
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
	if (!mesh) {
		return;
	}
	if (params["worldScale"].is_number()) {
		float scale = params["worldScale"].get<json::number_float_t>();
		mesh->SetWorldScale3D(FVector(scale, scale, scale));

	}
	float pitch = 0;
	float yaw = 0;
	float roll = 0;

	if (params["pitch"].is_number()) {
		pitch = params["pitch"].get<json::number_float_t>();
	}

	if (params["yaw"].is_number()) {
		yaw = params["yaw"].get<json::number_float_t>();
	}

	if (params["roll"].is_number()) {
		roll = params["roll"].get<json::number_float_t>();
	}

	UE_LOG(LogTemp, Warning, TEXT("Rotation: (%f, %f, %f)"), pitch, yaw, roll)

	mesh->SetWorldRotation(FRotator(pitch, yaw, roll));
	

	// setting random material
	if (params["useRandomTextures"].is_boolean() && params["useRandomTextures"]) {
		int32 numMats = mesh->GetNumMaterials();
		for (int32 i = 0; i < numMats; i++) {
			UMaterialInterface* material = getRandomMaterialFrom({ "/Game/ShapenetOBJ" }, true);
			if (material) {
			mesh->SetMaterial(i, material);
			}
		}
	}

	// setting physics
	if (params["physicsEnabled"].is_boolean() && !params["physicsEnabled"].get<bool>()) {
		mesh->SetSimulatePhysics(false);
	}
	else {
		mesh->SetSimulatePhysics(true);
		// damping for stable setup, to be removed after all actors have spawned and settled in position
		mesh->SetLinearDamping(20);
	}
	
	// setting gravity
	if (params["gravityEnabled"].is_boolean() && !params["gravityEnabled"].get<bool>()) {
		mesh->SetEnableGravity(false);
	}
	else {
		mesh->SetEnableGravity(true);
		// damping for stable setup, to be removed after all actors have spawned and settled in position
		mesh->SetLinearDamping(20);
	}

	// setting mass density
	//UPhysicalMaterial* physMat = baseMesh->GetBodySetup()->GetPhysMaterial();
	//physMat->Density = 10;

}

void AGymObj::locationSetup(FVector location, json::object_t params)
{
	bool autoHeight = true;
	if (params["autoHeight"].is_boolean()) {
		autoHeight = params["autoHeight"].get<json::boolean_t>();
	}
	if (autoHeight) {
		FVector origin;
		FVector extents;
		GetActorBounds(false, origin, extents);
		UE_LOG(LogTemp, Warning, TEXT("Autoheight"))
		// spawn object directly above floor (assuming automatic heigh adjustment for now
		location.Z = extents.Z + 1;
	}


	originalSpawnLocation = location;
	RootComponent->SetWorldLocation(location);
	

}


bool AGymObj::getInteractsWithGymObjs()
{
	return interactsWithGymObjs;
}
bool AGymObj::getInteractsWithGymAgents()
{
	return interactsWithGymAgents;
}

void AGymObj::setBaseMesh(UStaticMeshComponent* newBaseMesh)
{
	baseMesh = newBaseMesh;
}

void AGymObj::setName(FString newName)
{
	name = newName;
}

FString AGymObj::getName()
{
	return name;
}

void AGymObj::cook()
{
	// change meat texture
}

void AGymObj::addFire(FString mode)
{
	FString path = "/Game/StarterContent/Particles/P_Fire.P_Fire";
	UParticleSystem* particleSystem = Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), nullptr, *path));
	fire = NewObject<UParticleSystemComponent>(this, "fire");
	if (!particleSystem || !fire) {
		return;
	}
	fire->SetTemplate(particleSystem);
	fire->SetupAttachment(RootComponent);
	fire->RegisterComponent();
	fire->SetVisibility(false);
	if (mode == "indoor") {
		fire->SetRelativeLocation(FVector(0, 0, 10));
		fire->SetWorldScale3D(FVector(0.05, 0.05, 0.025));
	}
	else if (mode == "outdoor") {
		fire->SetRelativeLocation(FVector(-4.66, -19.33, -8));
		fire->SetRelativeScale3D(FVector(0.2, 0.5, 0.2));
	}

}

void AGymObj::toggleFire()
{
	if (fire->IsVisible()) {
		fire->SetVisibility(false);

	}
	else {
		fire->SetVisibility(true);
	}
}

void AGymObj::dispenseSalt()
{
	FString path = "/Game/Effects/Particles/Water/P_WaterSpill_Sm.P_WaterSpill_Sm";
	UParticleSystem* particleSystem = Cast<UParticleSystem>(StaticLoadObject(UParticleSystem::StaticClass(), nullptr, *path));
	salt = NewObject<UParticleSystemComponent>(this, "salt");
	salt->SetTemplate(particleSystem);
	salt->SetupAttachment(RootComponent);
	salt->RegisterComponent();
	salt->SetRelativeLocation(FVector(0, -90, 0));
}