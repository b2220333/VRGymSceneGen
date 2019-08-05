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

#include <random>

#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Engine/DirectionalLight.h" 
#include "Runtime/Engine/Classes/Engine/PointLight.h"
#include "GymObj.h"
#include "Runtime/Engine/Classes/Materials/Material.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"

ASceneGenUnrealGameModeBase::ASceneGenUnrealGameModeBase()
{
	
}

void ASceneGenUnrealGameModeBase::BeginPlay() {
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay: starting game mode"));
	spawnShapenetActors();

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
	// testing 
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	TArray<FAssetData> AssetData;
	AssetRegistryModule.Get().GetAssetsByClass(FName("MaterialInterface"), AssetData, true);
	UE_LOG(LogTemp, Warning, TEXT("Testing new mat found %d"), AssetData.Num());
	for (int32 i = 0; i < AssetData.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *AssetData[i].GetFullName());
	}


	TArray<UMaterialInterface*> materialAssets;
	TArray<FString> paths = { "/Game/" };
	AGymObj::getAssetsOfClass<UMaterialInterface>(materialAssets, paths, true);

	for (int32 i = 0; i < shapenetActors.Num(); i++) {
		if (shapenetActors[i]) {
			shapenetActors[i]->Destroy();
		}
	}
	shapenetActors.Empty();

	FString jsonPath = FPaths::ProjectDir() + "External/roomNew.json";

	IFileManager& FileManager = IFileManager::Get();
	FDateTime newDT = FileManager.GetTimeStamp(*jsonPath);

	if (roomJsonTimeStamp.GetTicks() < newDT.GetTicks()) {
		roomJsonTimeStamp = newDT;
		FString jsonString;
		FFileHelper::LoadFileToString(jsonString, *jsonPath);

		std::string jsonStr = std::string(TCHAR_TO_UTF8(*jsonString));

		parsed = json::parse(jsonStr);

		std::string dump = parsed.dump(4);
		FString dumpF = FString(dump.c_str());
		UE_LOG(LogTemp, Warning, TEXT("Testing new parse %s"), *dumpF);



		json::array_t& baseGroups = parsed["shapenetActorGroups"].get_ref<json::array_t&>();

		for (auto it = baseGroups.begin(); it != baseGroups.end(); it++) {
			if (it->is_object()) {
				json::object_t& baseGroup = it->get_ref<json::object_t&>();
				listDescendants(baseGroup);
				passDownParams(baseGroup);
			}
		}

		dump = parsed.dump(4);
		dumpF = FString(dump.c_str());
		UE_LOG(LogTemp, Warning, TEXT("Json with params passed down: %s"), *dumpF);

		// spawn floor (default on xy plane i.e. z = 0)

		float xWidth = 1000;
		float yWidth = 1000;
		float zWidth = 1000;
		if (parsed["xWidth"].is_number()) {
			xWidth = parsed["xWidth"].get<json::number_float_t>();
		}

		if (parsed["yWidth"].is_number()) {
			yWidth = parsed["yWidth"].get<json::number_float_t>();
		}

		if (parsed["zWidth"].is_number()) {
			yWidth = parsed["zWidth"].get<json::number_float_t>();
		}


		FVector spawnLocation = FVector(0, 0, 0);
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AShapenet* spawnedActor = GetWorld()->SpawnActor<AShapenet>(spawnLocation, FRotator::ZeroRotator, spawnParams);
		spawnedActor->spawnFloor(xWidth, yWidth);
	}	
	
	json::array_t& baseGroups = parsed["shapenetActorGroups"].get_ref<json::array_t&>();
	for (auto it = baseGroups.begin(); it != baseGroups.end(); it++) {
		importShapenetActorGroup(*it, FVector(0, 0, 0));
	}
	objectsDamped = true;
	

	/*
	for (int32 i = 5; i > 0; i--) {
		UE_LOG(LogTemp, Warning, TEXT("Waiting for actors to settle %d"), i);
		FPlatformProcess::Sleep(1);
	}
	resetDamping();
	*/

	// testing add light
	FVector lightLocation = FVector(0, 0, 50);
	FRotator lightRotation = FRotator(90, 0, 0);
	FActorSpawnParameters lightSpawnParams;
	ADirectionalLight* spawnedLight = GetWorld()->SpawnActor<ADirectionalLight>(lightLocation, lightRotation, lightSpawnParams);
	spawnedLight->GetRootComponent()->SetMobility(EComponentMobility::Movable);





	GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &ASceneGenUnrealGameModeBase::resetDamping, 5);
	
}

void ASceneGenUnrealGameModeBase::listDescendants(json::object_t& actorGroup)
{
	if (actorGroup["name"].is_string()) {
		std::string nameStr = actorGroup["name"].get<json::string_t>();
		FString str = FString(nameStr.c_str()) + " (";
		if (actorGroup["shapenetActors"].is_array()) {
			json::array_t& actors = actorGroup["shapenetActors"].get_ref<json::array_t&>();
			for (auto it = actors.begin(); it != actors.end(); it++) {
				if (it->is_object()) {
					json::object_t& obj = it->get_ref<json::object_t&>();
					if (obj["name"].is_string()) {
						nameStr = obj["name"].get<json::string_t>();
						str += FString(nameStr.c_str());
						str += ",";
					}
				}
				
			}
			str += ")";
		}
		UE_LOG(LogTemp, Warning, TEXT("listDescendents: %s ->"), *str);
		if (actorGroup["childShapenetActorGroups"].is_array()) {
			json::array_t& children = actorGroup["childShapenetActorGroups"].get_ref<json::array_t&>();
			for (auto it = children.begin(); it != children.end(); it++) {
				if (it->is_object()) {
					json::object_t& obj = it->get_ref<json::object_t&>();
					if (obj["name"].is_string()) {
						nameStr = obj["name"].get<json::string_t>();
						str = FString(nameStr.c_str());
						UE_LOG(LogTemp, Warning, TEXT("listDescendents: \t %s"), *str);
					}
				}
			}
			if (actorGroup["childShapenetActorGroups"].is_array()) {
				json::array_t& children = actorGroup["childShapenetActorGroups"].get_ref<json::array_t&>();
				for (auto it = children.begin(); it != children.end(); it++) {
					if (it->is_object()) {
						json::object_t& obj = it->get_ref<json::object_t&>();
						listDescendants(obj);
					}
				}
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("listDescendents: \t NONE"));
		}
	}
}



void ASceneGenUnrealGameModeBase::importShapenetActorGroup(json::object_t actorGroup, FVector origin)
{
	float relX = 0;
	float relY = 0;
	float relZ = 0;



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


	if (actorGroup["xDist"].is_object()) {
		relX = sampleLocation(actorGroup["xDist"].get_ref<json::object_t&>());
	}
	if (actorGroup["yDist"].is_object()) {
		relY = sampleLocation(actorGroup["yDist"].get_ref<json::object_t&>());
	}
	if (actorGroup["zDist"].is_object()) {
		relZ = sampleLocation(actorGroup["zDist"].get_ref<json::object_t&>());
	}

	FVector absoluteOrigin = origin + FVector(relX, relY, relZ);

	json::value_type actors = actorGroup["shapenetActors"];
	if (actors.is_array()) {
		for (auto it = actors.begin(); it != actors.end(); it++) {
			if (it->is_object()) {
				importShapenetActor(*it, absoluteOrigin);
			}
		}
	}

	json::value_type childGroups = actorGroup["childShapenetActorGroups"];

	if (childGroups.is_array()) {
		for (auto it = childGroups.begin(); it != childGroups.end(); it++) {
			if (it->is_object()) {
				importShapenetActorGroup(*it, absoluteOrigin);
			}
		}

	}

}

void ASceneGenUnrealGameModeBase::importShapenetActor(json::object_t actor, FVector origin)
{
	float relX = 0;
	float relY = 0;
	float relZ = 0;

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

	if (actor["xDist"].is_object()) {
		relX = sampleLocation(actor["xDist"].get_ref<json::object_t&>());
	}
	if (actor["yDist"].is_object()) {
		relY = sampleLocation(actor["yDist"].get_ref<json::object_t&>());
	}
	if (actor["zDist"].is_object()) {
		relZ = sampleLocation(actor["zDist"].get_ref<json::object_t&>());
	}

	FVector spawnLocation = FVector(origin.X + relX, origin.Y + relY, origin.Z + relZ) * FVector(-1.0, 1.0, 1.0);
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AShapenet* spawnedActor = GetWorld()->SpawnActor<AShapenet>(spawnLocation, FRotator::ZeroRotator, spawnParams);
	
	if (spawnedActor->importNew(spawnLocation, actor["actorParams"])) {
		shapenetActors.Add(spawnedActor);
		if (actor["name"].is_string()) {
			FString displayName = FString(actor["name"].get<json::string_t>().c_str());
			spawnedActor->SetActorLabel(displayName);
			UE_LOG(LogTemp, Warning, TEXT("Spawning %s at (%f %f, %f)"), *displayName, spawnLocation.X, spawnLocation.Y, spawnLocation.Z);
		}
	} else {
		spawnedActor->Destroy();

		// try again with lower CoM?

	}

}


void ASceneGenUnrealGameModeBase::resetDamping()
{
	for (int32 i = 0; i < shapenetActors.Num(); i++) {
		if (shapenetActors[i]) {
			shapenetActors[i]->tryRespawnNewCM();
			if (shapenetActors[i]->getBaseMesh()) {
				shapenetActors[i]->getBaseMesh()->SetLinearDamping(0);
			}
		}
	}
	objectsDamped = false;
	UE_LOG(LogTemp, Warning, TEXT("Ready to run episode"));
}



void ASceneGenUnrealGameModeBase::randomizeTextures(AShapenet shapenetActor)
{

}
void ASceneGenUnrealGameModeBase::randomizePosition(AShapenet shapenetActor, int32 constraints)
{

}




void ASceneGenUnrealGameModeBase::transferParamsBetween(json::object_t &srcObj, json::object_t &dstObj)
{
	if (srcObj["actorParams"].is_object()) {
		json::object_t& srcParams = srcObj["actorParams"].get_ref<json::object_t&>();
		if (!dstObj["actorParams"].is_object()) {
			dstObj["actorParams"] = json::object();
		}
		json::object_t& dstParams = dstObj["actorParams"].get_ref<json::object_t&>();
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
}

void ASceneGenUnrealGameModeBase::passDownParams(json::object_t &srcObj)
{
	std::string test = srcObj["name"].get<std::string>();
	FString testF = FString(test.c_str());


	if (srcObj["shapenetActors"].is_array()) {
		for (auto it = srcObj["shapenetActors"].begin(); it != srcObj["shapenetActors"].end(); it++) {
			if (it->is_object()) {
				json::object_t& ptr = it->get_ref<json::object_t&>();
				transferParamsBetween(srcObj, ptr);
			}

		}
	}

	if (srcObj["childShapenetActorGroups"].is_array()) {
		for (auto it = srcObj["childShapenetActorGroups"].begin(); it != srcObj["childShapenetActorGroups"].end(); it++) {
			if (it->is_object()) {
				json::object_t& child = it->get_ref<json::object_t&>();
				transferParamsBetween(srcObj, child);
				passDownParams(child);
			}
			
		}
	}

}

float ASceneGenUnrealGameModeBase::sampleLocation(json::object_t &location)
{
	if (location["uniform"].is_object()) {
		if (location["uniformContinuous"]["min"].is_number() && location["uniform"]["max"].is_number()) {
			float min = location["uniformContinuous"]["min"].get<json::number_float_t>();
			float max = location["uniformContinuous"]["min"].get<json::number_float_t>();
			return FMath::RandRange(min, max);
		}
	}
	else if (location["gaussian"].is_object()) {
		UE_LOG(LogTemp, Warning, TEXT("HERE"));
		if (location["gaussian"]["mean"].is_number() && location["gaussian"]["std"].is_number()) {
			float mean = location["gaussian"]["mean"].get<json::number_float_t>();
			float std = location["gaussian"]["std"].get<json::number_float_t>();
			std::random_device rd;
			std::default_random_engine generator(rd());
			std::normal_distribution<float> distribution(mean, std);
			return distribution(generator);
		}
	}
	else if (location["uniformDiscrete"].is_object()) {
		if (location["uniformDiscrete"]["min"].is_number() && location["uniformDiscrete"]["max"].is_number() && location["uniformDiscrete"]["step"].is_number()) {
			float min = location["uniformDiscrete"]["min"].get<json::number_float_t>();
			float max = location["uniformDiscrete"]["min"].get<json::number_float_t>();
			float step = location["uniformDiscrete"]["step"].get<json::number_float_t>();
			int numSteps = (max - min) / step + 1;
			int32 sampleSteps = FMath::RandRange(0, numSteps);
			return min + step * sampleSteps;
		}
	}
	else if (location["discrete"].is_object()) {
		float rng = FMath::RandRange(0.0f, 1.0f);
		if (location["discrete"]["probabilityDensities"].is_array()) {
			json::array_t& densities = location["discrete"]["probabilityDensities"].get_ref<json::array_t&>();
			float cummulative = 0;
			int index = 0;
			for (auto it = densities.begin(); it != densities.end(); it++) {
				if (it->is_number_float()) {
					float density = it->get<json::number_float_t>();
					if (cummulative + density > rng) {
						break;
					}
					else {
						cummulative += density;
					}
				}
				index++;
			}

			if (location["discrete"]["values"].is_array()) {
				json::array_t& values = location["discrete"]["values"].get_ref<json::array_t&>();

				int valuesIndex = 0;
				for (auto it = values.begin(); it != values.end(); it++) {
					if (valuesIndex == index && it->is_number()) {
						return it->get<json::number_float_t>();
					}
					valuesIndex++;
				}
			}
		}
	}
	return 0;
}

json::object_t ASceneGenUnrealGameModeBase::parseJsonFromPath(FString path)
{
	json::object_t jsonObj;
	FString jsonString;
	FFileHelper::LoadFileToString(jsonString, *path);
	std::string jsonStr = std::string(TCHAR_TO_UTF8(*jsonString));
	auto parsed = json::parse(jsonStr);

	if (parsed.is_object()) {
		jsonObj = parsed.get<json::object_t>();
	}
	return jsonObj;
}

json::array_t ASceneGenUnrealGameModeBase::parseJsonArrayFromPath(FString path)
{
	json::array_t jsonArray;
	FString jsonString;
	FFileHelper::LoadFileToString(jsonString, *path);
	std::string jsonStr = std::string(TCHAR_TO_UTF8(*jsonString));
	auto parsed = json::parse(jsonStr);

	if (parsed.is_array()) {
		jsonArray = parsed.get<json::array_t>();
	}
	return jsonArray;
}