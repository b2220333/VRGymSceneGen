// Fill out your copyright notice in the Description page of Project Settings.

#include "SceneGenUnrealGameModeBase.h"

#include "Shapenet.h"

#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Paths.h"

ASceneGenUnrealGameModeBase::ASceneGenUnrealGameModeBase()
{

}

void ASceneGenUnrealGameModeBase::BeginPlay() {
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay: starting game mode"));
	// generate scene here ?
	spawnShapenetActors();

}

void ASceneGenUnrealGameModeBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

}

void ASceneGenUnrealGameModeBase::spawnShapenetActors()
{
	FString jsonPath = FPaths::ProjectDir() + "External/room_template.json";;

	FString jsonString;
	FFileHelper::LoadFileToString(jsonString, *jsonPath);

	FRoomJson roomJson;

	bool parsed = FJsonObjectConverter::JsonObjectStringToUStruct(jsonString, &roomJson, 0, 0);
	if (parsed) {
		UE_LOG(LogTemp, Warning, TEXT("spawnShapenetActors: testing %s"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("spawnShapenetActors: Failed to parsed desired output"));
	}

	TArray<FShapenetActorGroup*> baseActorGroups = linkShapenetActorGroups(roomJson.shapenetActorGroups);

	for (int32 i = 0; i < baseActorGroups.Num(); i++) {
		importShapenetActorGroup(baseActorGroups[i], nullptr);
	}

}

void ASceneGenUnrealGameModeBase::importShapenetActorGroup(FShapenetActorGroup* actorGroup, FActorParams* params)
{
	FActorParams* importParams;
	if (!params) {
		importParams = &actorGroup->actorParams;
	}
	else {
		importParams = params;
	}
	for (int32 i = 0; i < actorGroup->shapenetActors.Num(); i++) {
		importShapenetActor(&actorGroup->shapenetActors[i], importParams);
	}


	for (int32 i = 0; i < actorGroup->childGroups.Num(); i++) {
		importShapenetActorGroup(actorGroup->childGroups[i], importParams);
	}
}

void ASceneGenUnrealGameModeBase::importShapenetActor(FShapenetActor* actor, FActorParams* params)
{

}


void ASceneGenUnrealGameModeBase::randomizeTextures(AShapenet shapenetActor)
{

}
void ASceneGenUnrealGameModeBase::randomizePosition(AShapenet shapenetActor, int32 constraints)
{

}

TArray<FShapenetActorGroup*> ASceneGenUnrealGameModeBase::linkShapenetActorGroups(TArray<FShapenetActorGroup> actorGroups)
{
	// linking group pointers
	for (int32 i = 0; i < actorGroups.Num(); i++) {
		for (int32 j = 0; j < actorGroups[i].childShapenetActorGroups.Num(); j++) {
			for (int32 k = 0; k < actorGroups.Num(); k++) {
				if (actorGroups[i].childShapenetActorGroups[j] == actorGroups[k].name) {
					actorGroups[k].parentGroup = &actorGroups[i];
					actorGroups[i].childGroups.Add(&actorGroups[k]);
				}
			}
		}
	}

	// finding base groups

	TArray<FShapenetActorGroup*> baseActorGroups;

	for (int32 i = 0; i < actorGroups.Num(); i++) {
		if (!actorGroups[i].parentGroup) {
			baseActorGroups.Add(&actorGroups[i]);
		}
	}

	return baseActorGroups;

}