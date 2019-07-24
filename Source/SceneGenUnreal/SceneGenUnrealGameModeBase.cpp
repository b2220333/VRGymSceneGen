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
		shapenetActors[i]->Destroy();
	}
	shapenetActors.Empty();

	FString jsonPath = FPaths::ProjectDir() + "External/roomNew.json";

	FString jsonString;
	FFileHelper::LoadFileToString(jsonString, *jsonPath);

	std::string jsonStr = std::string(TCHAR_TO_UTF8(*jsonString));

	auto newParsed = json::parse(jsonStr);

	std::string dump = newParsed.dump(4);

	FString dumpF = FString(dump.c_str());

	UE_LOG(LogTemp, Warning, TEXT("Testing new parse %s"), *dumpF);

	json::array_t baseGroups = newParsed["shapenetActorGroups"];

	for (auto it = baseGroups.begin(); it != baseGroups.end(); it++) {
		importShapenetActorGroupNew(*it, FVector(0, 0, 0));
	}
	


	jsonPath = FPaths::ProjectDir() + "External/room.json";
	FFileHelper::LoadFileToString(jsonString, *jsonPath);
	FRoomJson roomJson;

	bool parsed = FJsonObjectConverter::JsonObjectStringToUStruct(jsonString, &roomJson, 0, 0);

	if (!parsed) {
		UE_LOG(LogTemp, Warning, TEXT("spawnShapenetActors: Parse failed"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("spawnShapenetActors: Num actor groups is %d"), roomJson.shapenetActorGroups.Num());

	//UE_LOG(LogTemp, Warning, TEXT(" TESTING BEFORE %f"), roomJson.shapenetActorGroups[0].xCenter);

	TArray<FShapenetActorGroup*> baseActorGroups = linkShapenetActorGroups(&roomJson.shapenetActorGroups);
	
	UE_LOG(LogTemp, Warning, TEXT("spawnShapenetActors: Num base actor groups is %d"), baseActorGroups.Num());
	
	for (int32 i = 0; i < baseActorGroups.Num(); i++) {
		//UE_LOG(LogTemp, Warning, TEXT(" TESTING AFTER LINK %f"), baseActorGroups[i]->xCenter);
		listDescendants(baseActorGroups[i]);
	}

	
	/*
	FActorParams defaultParams;
	defaultParams.quantity = 1;
	defaultParams.spawnProbability = 1.0f;
	defaultParams.destructable = 1;
	defaultParams.physicsEnabled = 1;
	defaultParams.useRandomTextures = 0;
	defaultParams.useSameMeshForAllInstances = 1;
	defaultParams.useSameTextureForAllInstances = 1;
	defaultParams.canOverlap = 0;
	*/

	for (int32 i = 0; i < baseActorGroups.Num(); i++) {
		//transferParams(&defaultParams, &baseActorGroups[i]->actorParams);
		importShapenetActorGroup(baseActorGroups[i]);
	}
	
	//FPlatformProcess::Sleep(3);
	/*
	for (int32 i = 0; i < shapenetActors.Num(); i++) {
		
	


		UStaticMeshComponent* test = shapenetActors[i]->BaseMesh;
		FBox box = test->Bounds.GetBox();
		FVector extents = box.GetExtent();
		UE_LOG(LogTemp, Warning, TEXT("BoxExtent: (%f, %f, %f)"), extents.X, extents.Y, extents.Z);

	}
	*/


	json test;

	test["bob"] = 420;
	test["outerbob"]["innerbob"] = 69;
	int32 a = test["bob"];

	auto wtf = test.find("bob");

	if (wtf == test.end()) {
		UE_LOG(LogTemp, Warning, TEXT("did not find bob"))
	} else {
		UE_LOG(LogTemp, Warning, TEXT("found bob"))
	}

	auto wtf2 = test.find("elmo");

	if (wtf2 == test.end()) {
		UE_LOG(LogTemp, Warning, TEXT("did not find elmo"))
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("found elmo"))
	}

	

	UE_LOG(LogTemp, Warning, TEXT("test[\"bob\"] = %d"), a);

	auto testParse = json::parse("{ \"hello\" : \"world\"}");

	std::string output = testParse["hello"];

	FString testOutput = FString(output.c_str());

	UE_LOG(LogTemp, Warning, TEXT("testParse[\"hello\"] = %s"), *testOutput);

	FString lul = "whoami";

	std::string lulstr = std::string(TCHAR_TO_UTF8(*lul));

	//15959
	testParse[lulstr] = "skynet";

	std::string output2 = testParse["whoami"];

	FString testOutput2 = FString(output2.c_str());

	UE_LOG(LogTemp, Warning, TEXT("testParse[\"hello\"] = %s"), *testOutput2);

	json::value_type testV = testParse[lulstr];

	json::object_t testabc;

	json::array_t tarr;

	tarr.size();

	if (testV.is_string()) {
		UE_LOG(LogTemp, Warning, TEXT("Is a string "));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Is not a string "));
	}

	testParse["arrayHere"] = {1,2,3};

	json::array_t testArr = testParse["arrayHere"];

	for (auto it = testArr.begin(); it != testArr.end(); it++) {
		int32 z = *it;
		UE_LOG(LogTemp, Warning, TEXT("Testing iterator: %d"), z);
	}


	

	

	
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
	if (x.is_number_float()) {
		relX = x;
	}
	json::value_type y = actorGroup["yCenter"];
	if (y.is_number_float()) {
		relY = y;
	}
	json::value_type z = actorGroup["zCenter"];
	if (z.is_number_float()) {
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
	if (x.is_number_float()) {
		relX = x;
	}
	json::value_type y = actor["y"];
	if (y.is_number_float()) {
		relY = y;
	}
	json::value_type z = actor["z"];
	if (z.is_number_float()) {
		relZ = z;
	}

	FVector spawnLocation = FVector(origin.X + relX, origin.Y + relY, origin.Z + relZ) * FVector(-1.0, 1.0, 1.0);;
	FActorSpawnParameters spawnParams;
	AShapenet* spawnedActor = GetWorld()->SpawnActor<AShapenet>(spawnLocation, FRotator::ZeroRotator, spawnParams);
	
	json::value_type name = actor["name"];
	if (name.is_string()) {
		std::string nameStr = actor["name"];
		FString displayName = "TestShapenet-" + FString(nameStr.c_str());
		spawnedActor->SetActorLabel(displayName);
	}

	json::value_type syn = actor["synset"];
	if (syn.is_string()) {
		std::string synStr = actor["synset"];
		FString synset = FString(synStr.c_str());
		if (actor["actorParams"].is_object()) {
			spawnedActor->importRandomFromSynsetNew(synset, spawnLocation, actor["actoParams"]);
		}
	}

	
	

	shapenetActors.Add(spawnedActor);
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