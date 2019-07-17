// Fill out your copyright notice in the Description page of Project Settings.

#include "SceneGenUnrealGameModeBase.h"

#include "Shapenet.h"

#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Paths.h"


#include "Runtime/Engine/Classes/Engine/World.h"

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
	FString jsonPath = FPaths::ProjectDir() + "External/room.json";;

	FString jsonString;
	FFileHelper::LoadFileToString(jsonString, *jsonPath);

	FRoomJson roomJson;

	bool parsed = FJsonObjectConverter::JsonObjectStringToUStruct(jsonString, &roomJson, 0, 0);

	if (!parsed) {
		UE_LOG(LogTemp, Warning, TEXT("spawnShapenetActors: Parse failed"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("spawnShapenetActors: Num actor groups is %d"), roomJson.shapenetActorGroups.Num());

	TArray<FShapenetActorGroup*> baseActorGroups = linkShapenetActorGroups(&roomJson.shapenetActorGroups);
	
	UE_LOG(LogTemp, Warning, TEXT("spawnShapenetActors: Num base actor groups is %d"), baseActorGroups.Num());
	
	for (int32 i = 0; i < baseActorGroups.Num(); i++) {
		listDescendants(baseActorGroups[i]);
	}

	/*
	for (int32 i = 0; i < baseActorGroups.Num(); i++) {
		baseActorGroups[i]->groupOrigin = FVector(baseActorGroups[i]->xCenter, baseActorGroups[i]->yCenter, baseActorGroups[i]->zCenter);
		importShapenetActorGroup(baseActorGroups[i], nullptr);
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
		importShapenetActor(&actorGroup->shapenetActors[i], importParams, &actorGroup->groupOrigin);
	}


	for (int32 i = 0; i < actorGroup->childGroups.Num(); i++) {
		actorGroup->childGroups[i]->groupOrigin += FVector(actorGroup->childGroups[i]->xCenter, actorGroup->childGroups[i]->yCenter, actorGroup->childGroups[i]->zCenter);
		importShapenetActorGroup(actorGroup->childGroups[i], importParams);
	}
}

void ASceneGenUnrealGameModeBase::importShapenetActor(FShapenetActor* actor, FActorParams* params, FVector* origin)
{
	FVector spawnLocation = FVector(actor->x, actor->y, actor->z) + *origin;
	FActorSpawnParameters SpawnInfo;
	AShapenet* spawnedActor = GetWorld()->SpawnActor<AShapenet>(spawnLocation, FRotator::ZeroRotator, SpawnInfo);
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