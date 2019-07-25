// Fill out your copyright notice in the Description page of Project Settings.

#include "SceneGenUnrealGameModeBase.h"

#include "Shapenet.h"

#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Paths.h"


#include "Runtime/Engine/Classes/Engine/World.h"

#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/ObjectLibrary.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "Runtime/AssetRegistry/Public/IAssetRegistry.h"

#include "json.hpp"
using json = nlohmann::json;

ASceneGenUnrealGameModeBase::ASceneGenUnrealGameModeBase()
{

}

void ASceneGenUnrealGameModeBase::BeginPlay() {
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay: starting game mode"));
	// generate scene here ?
	spawnShapenetActors();

	//InputComponent->BindAction("resampleScene", IE_Pressed, this, &ASceneGenUnrealGameModeBase::spawnShapenetActors);
	UWorld* World = GetWorld();
	if (World) {
		World->GetFirstPlayerController()->InputComponent->BindAction("resampleScene", IE_Pressed, this, &ASceneGenUnrealGameModeBase::spawnShapenetActors);
	}
}

void ASceneGenUnrealGameModeBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

}

void ASceneGenUnrealGameModeBase::spawnShapenetActors()
{
	for (int32 i = 0; i < shapenetActors.Num(); i++) {
		if (shapenetActors[i]) {
			shapenetActors[i]->Destroy();
		}
	}
	shapenetActors.Empty();

	FString jsonPath = FPaths::ProjectDir() + "External/roomNew.json";

	FString jsonString;
	FFileHelper::LoadFileToString(jsonString, *jsonPath);

	std::string jsonStr = std::string(TCHAR_TO_UTF8(*jsonString));

	json newParsed = json::parse(jsonStr);

	std::string dump = newParsed.dump(4);
	FString dumpF = FString(dump.c_str());
	UE_LOG(LogTemp, Warning, TEXT("Testing new parse %s"), *dumpF);


	json::array_t baseGroups = newParsed["shapenetActorGroups"].get_ref<json::array_t&>();

	for (auto it = baseGroups.begin(); it != baseGroups.end(); it++) {
		json::object_t baseGroup = it->get_ref<json::object_t&>();
		passDownParams(baseGroup);
		importShapenetActorGroupNew(*it, FVector(0, 0, 0));
	}
	
	dump = newParsed.dump(4);
	dumpF = FString(dump.c_str());
	UE_LOG(LogTemp, Warning, TEXT("Testing new parse passed down %s"), *dumpF);

	jsonPath = FPaths::ProjectDir() + "External/room.json";
	FFileHelper::LoadFileToString(jsonString, *jsonPath);
	FRoomJson roomJson;

	bool parsed = FJsonObjectConverter::JsonObjectStringToUStruct(jsonString, &roomJson, 0, 0);

	
	
	//FPlatformProcess::Sleep(3);
	/*
	for (int32 i = 0; i < shapenetActors.Num(); i++) {
		
	


		UStaticMeshComponent* test = shapenetActors[i]->BaseMesh;
		FBox box = test->Bounds.GetBox();
		FVector extents = box.GetExtent();
		UE_LOG(LogTemp, Warning, TEXT("BoxExtent: (%f, %f, %f)"), extents.X, extents.Y, extents.Z);

	}
	*/
	
}

void ASceneGenUnrealGameModeBase::listDescendants(FShapenetActorGroup* actorGroup)
{
	FString str = actorGroup->name + " (";
	for (int32 i = 0; i < actorGroup->shapenetActors.Num();  i++) {
		str += actorGroup->shapenetActors[i].name;
		if (i < actorGroup->shapenetActors.Num() - 1) {
			str += ",";
		}
	}
	str += ")";
	UE_LOG(LogTemp, Warning, TEXT("listDescendents: %s ->"), *str);
	if (actorGroup->childGroups.Num() > 0) {
		for (int32 i = 0; i < actorGroup->childGroups.Num(); i++) {
			str = actorGroup->childGroups[i]->name;
			UE_LOG(LogTemp, Warning, TEXT("listDescendents: \t %s"), *str);
			
		}
		for (int32 i = 0; i < actorGroup->childGroups.Num(); i++) {
			listDescendants(actorGroup->childGroups[i]);
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("listDescendents: \t NONE"));
	}
}

void ASceneGenUnrealGameModeBase::importShapenetActorGroup(FShapenetActorGroup* actorGroup)
{

	actorGroup->groupOrigin += FVector(actorGroup->xCenter, actorGroup->yCenter, actorGroup->zCenter);
	for (int32 i = 0; i < actorGroup->shapenetActors.Num(); i++) {
		//UE_LOG(LogTemp, Warning, TEXT("TEST %s origin is at (%d, %d, %d)"), *actorGroup->name, actorGroup->groupOrigin.X, actorGroup->groupOrigin.Y, actorGroup->groupOrigin.Z);
		transferParams(&actorGroup->actorParams, &actorGroup->shapenetActors[i].actorParams);
		importShapenetActor(&actorGroup->shapenetActors[i], &actorGroup->groupOrigin);
	}


	for (int32 i = 0; i < actorGroup->childGroups.Num(); i++) {
		actorGroup->childGroups[i]->groupOrigin += FVector(actorGroup->groupOrigin.X, actorGroup->groupOrigin.Y, actorGroup->groupOrigin.Z);
		transferParams(&actorGroup->actorParams, &actorGroup->childGroups[i]->actorParams);
		importShapenetActorGroup(actorGroup->childGroups[i]);
	}
}

void ASceneGenUnrealGameModeBase::importShapenetActorGroupNew(json::object_t actorGroup, FVector origin)
{
	int32 relX, relY, relZ;


	json::value_type x = actorGroup["xCenter"];
	if (x.is_number()) {
		relX = x;
	}
	json::value_type y = actorGroup["yCenter"];
	if (y.is_number()) {
		relY = y;
	}
	json::value_type z = actorGroup["zCenter"];
	if (z.is_number()) {
		relZ = z;
	}

	FVector absoluteOrigin = origin + FVector(relX, relY, relZ);

	json::value_type actors = actorGroup["shapenetActors"];
	if (actors.is_array()) {
		for (auto it = actors.begin(); it != actors.end(); it++) {
			if (it->is_object()) {
				importShapenetActorNew(*it, absoluteOrigin);
			}
		}
	}

	json::value_type childGroups = actorGroup["childShapenetActorGroups"];

	if (childGroups.is_array()) {
		for (auto it = childGroups.begin(); it != childGroups.end(); it++) {
			if (it->is_object()) {
				importShapenetActorGroupNew(*it, absoluteOrigin);
			}
		}

	}

}

void ASceneGenUnrealGameModeBase::importShapenetActorNew(json::object_t actor, FVector origin)
{
	int32 relX, relY, relZ;

	json::value_type x = actor["x"];
	if (x.is_number()) {
		relX = x;
	}
	json::value_type y = actor["y"];
	if (y.is_number()) {
		relY = y;
	}
	json::value_type z = actor["z"];
	if (z.is_number()) {
		relZ = z;
	}

	FVector spawnLocation = FVector(origin.X + relX, origin.Y + relY, origin.Z + relZ) * FVector(-1.0, 1.0, 1.0);
	FActorSpawnParameters spawnParams;
	
	AShapenet* spawnedActor = GetWorld()->SpawnActor<AShapenet>(spawnLocation, FRotator::ZeroRotator, spawnParams);
	
	json::value_type name = actor["name"];
	if (name.is_string()) {
		std::string nameStr = actor["name"];
		FString displayName = "TestShapenet-" + FString(nameStr.c_str());
		spawnedActor->SetActorLabel(displayName);
	}

	if (actor["actorParams"].is_object() && actor["actorParams"]["shapenetSynset"].is_string()) {
		std::string syn = actor["actorParams"]["shapenetSynset"];
		FString synset = FString(syn.c_str());
		spawnedActor->importRandomFromSynsetNew(synset, spawnLocation, actor["actorParams"]);
		UE_LOG(LogTemp, Warning, TEXT("Spawning at (%f %f, %f)"), spawnLocation.X, spawnLocation.Y, spawnLocation.Z);
		shapenetActors.Add(spawnedActor);
	}
}



void ASceneGenUnrealGameModeBase::importShapenetActor(FShapenetActor* actor,  FVector* origin)
{

	//UE_LOG(LogTemp, Warning, TEXT("Actor is at (%d, %d, %d)"), actor->x, actor->y, actor->z)
	//UE_LOG(LogTemp, Warning, TEXT("Origin is at (%d, %d, %d)"), origin->X, origin->Y, origin->Z)

	FTransform spawnTransfrom = FTransform(FVector(actor->x + origin->X, actor->y + origin->Y, actor->z + origin->Z));
	UE_LOG(LogTemp, Warning, TEXT("Spawning %s at (%f %f, %f)"), *actor->name, spawnTransfrom.GetTranslation().X, spawnTransfrom.GetTranslation().Y, spawnTransfrom.GetTranslation().Z);
	FActorSpawnParameters spawnParams;
	//spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AShapenet* spawnedActor = GetWorld()->SpawnActor<AShapenet>(spawnTransfrom.GetTranslation(), FRotator::ZeroRotator, spawnParams);
	FString displayName = "Shapenet-" + actor->name;
	spawnedActor->SetActorLabel(displayName);
	FVector location = FVector(actor->x + origin->X, actor->y + origin->Y, actor->z + origin->Z) * FVector(-1.0,1.0,1.0);
	// set up mesh and textures here 
	//spawnedActor->importMesh("02818832", "2f44a88e17474295e66f707221b74f43", location);

	spawnedActor->importRandomFromSynset(actor->actorParams.shapenetSynset, location, &actor->actorParams);

	shapenetActors.Add(spawnedActor);

	//UE_LOG(LogTemp, Warning, TEXT("SpawnProb: %f"), actor->actorParams.spawnProbability);
	
}

void ASceneGenUnrealGameModeBase::transferParams(FActorParams* parentParams, FActorParams* childParams)
{
	if (!parentParams || !childParams) {
		return;
	}
	if (childParams->shapenetSynset == "") {
		childParams->shapenetSynset = parentParams->shapenetSynset;
	}
	if (childParams->shapenetHash == "") {
		childParams->shapenetHash = parentParams->shapenetHash;
	}
	if (childParams->meshFromOverride == "") {
		childParams->meshFromOverride = parentParams->meshFromOverride;
	}
	if (childParams->textureOverride == "") {
		childParams->textureOverride = parentParams->textureOverride;
	}

	if (childParams->spawnProbability == -1.0f) {
		childParams->spawnProbability = parentParams->spawnProbability;
	}

	if (childParams->destructable == -1) {
		childParams->destructable = parentParams->destructable;
	}
	if (childParams->physicsEnabled == -1) {
		childParams->physicsEnabled = parentParams->physicsEnabled;
	}
	if (childParams->useRandomTextures == -1) {
		childParams->useRandomTextures = parentParams->useRandomTextures;
	}
	if (childParams->useSameMeshForAllInstances == -1) {
		childParams->useSameMeshForAllInstances = parentParams->useSameMeshForAllInstances;
	}
	if (childParams->useSameTextureForAllInstances == -1) {
		childParams->useSameTextureForAllInstances = parentParams->useSameTextureForAllInstances;
	}
	if (childParams->canOverlap == -1) {
		childParams->canOverlap = parentParams->canOverlap;
	}
	


}


void ASceneGenUnrealGameModeBase::randomizeTextures(AShapenet shapenetActor)
{

}
void ASceneGenUnrealGameModeBase::randomizePosition(AShapenet shapenetActor, int32 constraints)
{

}

TArray<FShapenetActorGroup*> ASceneGenUnrealGameModeBase::linkShapenetActorGroups(TArray<FShapenetActorGroup>* actorGroups)
{

	UE_LOG(LogTemp, Warning, TEXT("linkShapenetActorGroups: Starting linking process"));

	// linking group pointers
	for (int32 i = 0; i < actorGroups->Num(); i++) {
		FString name = (*actorGroups)[i].name;
		UE_LOG(LogTemp, Warning, TEXT("linkShapenetActorGroups: linking %s"), *name);
		for (int32 j = 0; j < (*actorGroups)[i].childShapenetActorGroups.Num(); j++) {
			for (int32 k = 0; k < actorGroups->Num(); k++) {
				if ((*actorGroups)[i].childShapenetActorGroups[j] == (*actorGroups)[k].name) {
					(*actorGroups)[k].parentGroup = &(*actorGroups)[i];
					(*actorGroups)[i].childGroups.Add(&(*actorGroups)[k]);
				}
			}
		}
	}

	// finding base groups

	TArray<FShapenetActorGroup*> baseActorGroups;

	for (int32 i = 0; i < actorGroups->Num(); i++) {
		if (!(*actorGroups)[i].parentGroup) {			
			baseActorGroups.Add(&(*actorGroups)[i]);
		}
	}

	return baseActorGroups;

}


void ASceneGenUnrealGameModeBase::transferParamsBetween(json::object_t &srcObj, json::object_t &dstObj)
{
	json::object_t srcParams;
	json::object_t dstParams;
	if (srcObj["actorParams"].is_object()) {
		srcParams = srcObj["actorParams"].get_ref<json::object_t&>();
		if (dstObj["actorParams"].is_object()) {
			dstParams = dstObj["actorParams"].get_ref<json::object_t&>();
		}
		else {
			dstParams["actorParams"] = json::object();
		}
	} else {
		return;
	}

	if (dstParams.find("shapenetSynset") == dstParams.end() && srcParams.find("shapenetSynset") != srcParams.end()) {
		dstParams["shapenetSynset"] = srcParams["shapenetSynset"];
	}

	if (dstParams.find("shapenetHash") == dstParams.end() && srcParams.find("shapenetHash") != srcParams.end()) {
		dstParams["shapenetHash"] = srcParams["shapenetHash"];
	}

	if (dstParams.find("meshOverride") == dstParams.end() && srcParams.find("meshOverride") != srcParams.end()) {
		dstParams["meshOverride"] = srcParams["meshOverride"];
	}

	if (dstParams.find("textureOverride") == dstParams.end() && srcParams.find("textureOverride") != srcParams.end()) {
		dstParams["textureOverride"] = srcParams["textureOverride"];
	}

	if (dstParams.find("spawnProbability") == dstParams.end() && srcParams.find("spawnProbability") != srcParams.end()) {
		UE_LOG(LogTemp, Warning, TEXT("here"))
		
		dstParams["spawnProbability"] = srcParams["spawnProbability"];
	}

	if (dstParams.find("destructable") == dstParams.end() && srcParams.find("destructable") != srcParams.end()) {
		dstParams["destructable"] = srcParams["destructable"];
	}

	if (dstParams.find("physicsEnabled") == dstParams.end() && srcParams.find("physicsEnabled") != srcParams.end()) {
		dstParams["physicsEnabled"] = srcParams["physicsEnabled"];
	}

	if (dstParams.find("useRandomTextures") == dstParams.end() && srcParams.find("useRandomTextures") != srcParams.end()) {
		dstParams["useRandomTextures"] = srcParams["useRandomTextures"];
	}

	if (dstParams.find("canOverlap") == dstParams.end() && srcParams.find("canOverlap") != srcParams.end()) {
		dstParams["canOverlap"] = srcParams["canOverlap"];
	}

}

void ASceneGenUnrealGameModeBase::passDownParams(json::object_t &srcObj)
{
	std::string test = srcObj["name"].get<std::string>();
	FString testF = FString(test.c_str());
	UE_LOG(LogTemp, Warning, TEXT("Testing %s"), *testF);


	if (srcObj["shapenetActors"].is_array()) {
		json::array_t actors = srcObj["shapenetActors"].get_ref<json::array_t&>();
		for (auto it = actors.begin(); it != actors.end(); it++) {
			if (it->is_object()) {
				json::object_t actor = it->get_ref<json::object_t&>();
				transferParamsBetween(srcObj, actor);
			}

		}
	}

	if (srcObj["childShapenetActorGroups"].is_array()) {
		json::array_t children = srcObj["childShapenetActorGroups"].get_ref<json::array_t&>();
		for (auto it = children.begin(); it != children.end(); it++) {
			if (it->is_object()) {
				json::object_t child = it->get_ref<json::object_t&>();
				transferParamsBetween(srcObj, child);
				passDownParams(child);
			}
			
		}
	}

}