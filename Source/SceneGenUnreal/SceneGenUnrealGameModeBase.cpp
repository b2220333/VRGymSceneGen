// Fill out your copyright notice in the Description page of Project Settings.

#include "SceneGenUnrealGameModeBase.h"

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
#include "GymAgent.h"
#include "GShapenet.h"
#include "GPartnet.h"
#include "GLighting.h"
#include "GWall.h"
#include "Runtime/Engine/Classes/Materials/Material.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "GDemoAgent.h"

ASceneGenUnrealGameModeBase::ASceneGenUnrealGameModeBase()
{
	DefaultPawnClass = AGDemoAgent::StaticClass();
}

void ASceneGenUnrealGameModeBase::BeginPlay() {
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay: starting game mode"));
	numResets = 0;
	spawnShapenetActors();
	UWorld* World = GetWorld();
	if (World) {
		World->GetFirstPlayerController()->InputComponent->BindAction("resampleScene", IE_Pressed, this, &ASceneGenUnrealGameModeBase::spawnShapenetActors);
		World->GetFirstPlayerController()->InputComponent->BindAction("toggleFire", IE_Pressed, this, &ASceneGenUnrealGameModeBase::toggleFire);
		World->GetFirstPlayerController()->InputComponent->BindAction("salt", IE_Pressed, this, &ASceneGenUnrealGameModeBase::dispenseSalt);
		World->GetFirstPlayerController()->InputComponent->BindAction("cook", IE_Pressed, this, &ASceneGenUnrealGameModeBase::cook);
		World->GetFirstPlayerController()->InputComponent->BindAction("disablePan", IE_Pressed, this, &ASceneGenUnrealGameModeBase::disablePan);
		World->GetFirstPlayerController()->InputComponent->BindAction("toggleLighter", IE_Pressed, this, &ASceneGenUnrealGameModeBase::toggleLighter);

	}

	
}

void ASceneGenUnrealGameModeBase::disablePan()
{
	pan = nullptr;
}


void ASceneGenUnrealGameModeBase::toggleFire()
{
	if (heatSource) {
		heatSource->toggleFire();
	}
	if (wood) {
		wood->toggleFire();
	}

	
}

void ASceneGenUnrealGameModeBase::toggleLighter()
{
	if (lighter) {
		lighter->toggleLighter();
	}
}


void ASceneGenUnrealGameModeBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
}



void ASceneGenUnrealGameModeBase::spawnShapenetActors()
{
	if (mode == "outdoor") {
		UE_LOG(LogTemp, Warning, TEXT("Outdoor"))
			FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		// spawn wood oven with fire 
		FString path = "/Game/DemoAssets/Stoves/outdoor/model_normalized.model_normalized";
		json::object_t actorParams;
		FVector spawnLocation = FVector(745, -2740, 60);
		actorParams["yaw"] = -90;
		actorParams["physicsEnabled"] = false;
		actorParams["autoHeight"] = false;
		AGymObj* spawnedGymObj = GetWorld()->SpawnActor<AGymObj>(spawnLocation, FRotator::ZeroRotator, spawnParams);
		spawnedGymObj->assignMeshFromPath(path, spawnLocation, actorParams);
		spawnedGymObj->SetActorScale3D(FVector(3.0, 2.5, 2.5));
		spawnedGymObj->addFire("outdoor");
		spawnedGymObj->SetActorLabel("heatSource");
		heatSource = spawnedGymObj;
		gymObjects.Add(spawnedGymObj);


		actorParams["physicsEnabled"] = true;
		// spawn lighter
		path = "/Game/DemoAssets/Lighter/Clipper.Clipper";
		actorParams["yaw"] = 0;
		actorParams["worldScale"] = 0.25;
		spawnLocation = FVector(620, -2320, 15);
		spawnedGymObj = GetWorld()->SpawnActor<AGymObj>(spawnLocation, FRotator::ZeroRotator, spawnParams);
		spawnedGymObj->originalRotation = FRotator(0, 0, 0);
		spawnedGymObj->assignMeshFromPath(path, spawnLocation, actorParams);
		spawnedGymObj->SetActorLabel("lighter");
		spawnedGymObj->addFire("lighter");
		lighter = spawnedGymObj;
		gymObjects.Add(spawnedGymObj);

		// spawn salt
		path = "/Game/Scenes/Small_Items/MinorObject/consistent/sugarpourer/SugarDespenseClassic.SugarDespenseClassic";
		spawnLocation = FVector(610, -2270, 15);
		actorParams["roll"] = 90;
		actorParams["worldScale"] = 0.15;
		spawnedGymObj = GetWorld()->SpawnActor<AGymObj>(spawnLocation, FRotator::ZeroRotator, spawnParams);
		spawnedGymObj->originalRotation = FRotator(0, 0, 90);
		spawnedGymObj->assignMeshFromPath(path, spawnLocation, actorParams);
		spawnedGymObj->SetActorLabel("salt");
		salt = spawnedGymObj;
		gymObjects.Add(spawnedGymObj);

		// spawn meat
		path = "/Game/Assets/Meat/meat.meat";
		spawnLocation = FVector(620, -2220, 15);
		actorParams["roll"] = 90;
		actorParams["worldScale"] = 2;
		spawnedGymObj = GetWorld()->SpawnActor<AGymObj>(spawnLocation, FRotator::ZeroRotator, spawnParams);
		spawnedGymObj->originalRotation = FRotator(0, 0, 90);
		spawnedGymObj->assignMeshFromPath(path, spawnLocation, actorParams);
		spawnedGymObj->SetActorLabel("meat");
		meat = spawnedGymObj;
		gymObjects.Add(spawnedGymObj);

		path = "/Game/DemoAssets/Wood/12303_Firewood_Stack_v1_l3.12303_Firewood_Stack_v1_l3";
		spawnLocation = FVector(-160, -3170, -70);
		actorParams["roll"] = 0;
		actorParams["yaw"] = -90;
		actorParams["worldScale"] = 1;
		spawnedGymObj = GetWorld()->SpawnActor<AGymObj>(spawnLocation, FRotator::ZeroRotator, spawnParams);
		spawnedGymObj->originalRotation = FRotator(0, 0, 90);
		spawnedGymObj->assignMeshFromPath(path, spawnLocation, actorParams);
		spawnedGymObj->SetActorLabel("wood");
		wood = spawnedGymObj;
		wood->addFire("");
		gymObjects.Add(spawnedGymObj);


		path = "/Game/Assets/sunCG/kitchenUtilities/cutboard/245Knife.245Knife";
		spawnLocation = FVector(640, -2470, 15);
		actorParams["pitch"] = 0;
		actorParams["yaw"] = 50;
		actorParams["roll"] = 90;
		actorParams["worldScale"] = 1;
		spawnedGymObj = GetWorld()->SpawnActor<AGymObj>(spawnLocation, FRotator::ZeroRotator, spawnParams);
		spawnedGymObj->originalRotation = FRotator(0, 50, 90);
		spawnedGymObj->assignMeshFromPath(path, spawnLocation, actorParams);
		spawnedGymObj->SetActorLabel("knife");
		knife = spawnedGymObj;
		gymObjects.Add(spawnedGymObj);





	} else {

		UE_LOG(LogTemp, Warning, TEXT("Outdoor"))


			for (int32 i = 0; i < gymObjects.Num(); i++) {
				if (gymObjects[i]) {
					gymObjects[i]->Destroy();
				}
			}
		gymObjects.Empty();
		for (int32 i = 0; i < gymAgents.Num(); i++) {
			if (gymAgents[i]) {
				gymAgents[i]->Destroy();
			}
		}
		gymAgents.Empty();
		if (primaryAgent) {
			primaryAgent = nullptr;
		}

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
				zWidth = parsed["zWidth"].get<json::number_float_t>();
			}
			autoSpawnWalls(true, true);
		}

		json::array_t& baseGroups = parsed["shapenetActorGroups"].get_ref<json::array_t&>();
		for (auto it = baseGroups.begin(); it != baseGroups.end(); it++) {
			importShapenetActorGroup(*it, FVector(0, 0, 0));
		}
		objectsDamped = true;

		/*
		// spawn params for test objects
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// spawn demo agent
		FVector agentSpawnLoc(310, -80, 140);
		AGDemoAgent* demoAgent = GetWorld()->SpawnActor<AGDemoAgent>(agentSpawnLoc, FRotator::ZeroRotator, spawnParams);
		if (demoAgent) {
			if (numResets != 0) {
				demoAgent->playRandomAnimation();
			}

			primaryAgent = demoAgent;
			gymAgents.Add(demoAgent);
		}

		// testing partnet models
		FVector spawnLocation = FVector(0, 0, 300);
		AGymObj* test = GetWorld()->SpawnActor<AGymObj>(spawnLocation, FRotator::ZeroRotator, spawnParams);
		json::object_t testparams;
		//testparams["physicsEnabled"] = false;
		test->assignMeshFromPath("/Game/partnetOBJ/Chair/36366/new-0.new-0", spawnLocation, testparams);
		test->assignMeshesFromPath("/Game/partnetOBJ/Chair/36366", spawnLocation, testparams);
		gymObjects.Add(test);

		// lighting
		AGLighting* light = GetWorld()->SpawnActor<AGLighting>(spawnLocation, FRotator::ZeroRotator, spawnParams);
		light->spawnPointLight(FVector(0, 0, 400));
		gymObjects.Add(light);

		*/

		// resets damping after 5 seconds to allow models to settle
	}
	GetWorld()->GetTimerManager().SetTimer(FuzeTimerHandle, this, &ASceneGenUnrealGameModeBase::resetDamping, 5);
	numResets++;
}

void ASceneGenUnrealGameModeBase::autoSpawnWalls(bool autoSpawnSideWalls, bool autoSpawnCeiling)
{
	// -z always spawn floor first 
	float xWidth = 0;
	float yWidth = 0;
	if (parsed.is_object() && parsed["xWidth"].is_number() && parsed["yWidth"].is_number()) {
		xWidth = parsed["xWidth"].get<json::number_float_t>();
		yWidth = parsed["xWidth"].get<json::number_float_t>();
	}
	FVector spawnLocation = FVector(0, 0, 0);
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AGWall* floor = GetWorld()->SpawnActor<AGWall>(spawnLocation, FRotator::ZeroRotator, spawnParams);
	floor->spawnFloor(xWidth, yWidth);
	//floor->applyDemoWallParams();

	float height = 0;
	if (autoSpawnSideWalls || autoSpawnCeiling) {
		if (parsed.is_object() && parsed["zWidth"].is_number()) {
			height = parsed["zWidth"].get<json::number_float_t>();
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Cannot auto spawn side walls or ceiling since no zwidth specified"))
			return;
		}
	}

	// spawn side walls
	if (autoSpawnSideWalls) {
		FVector spawnLocation = FVector(0, 0, 0);
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		// -x
		AGWall* wall = GetWorld()->SpawnActor<AGWall>(spawnLocation, FRotator::ZeroRotator, spawnParams);
		FTransform wallTransform;
		wallTransform.SetRotation(FQuat(FRotator(180, 0, 0)));
		wallTransform.SetTranslation(FVector(-xWidth/2, 0, height / 2));
		wallTransform.SetScale3D(FVector(1, xWidth / 256, height / 256));
		wall->spawnWall(wallTransform);
		wall->applyDemoWallParams();

		// +x
		wall = GetWorld()->SpawnActor<AGWall>(spawnLocation, FRotator::ZeroRotator, spawnParams);
		wallTransform.SetRotation(FQuat(FRotator(0, 0, 0)));
		wallTransform.SetTranslation(FVector(xWidth / 2, 0, height / 2));
		wallTransform.SetScale3D(FVector(1, xWidth / 256, height / 256));
		wall->spawnWall(wallTransform);
		wall->applyDemoWallParams();

		// -y
		wall = GetWorld()->SpawnActor<AGWall>(spawnLocation, FRotator::ZeroRotator, spawnParams);
		wallTransform.SetRotation(FQuat(FRotator(0, -90, 0)));
		wallTransform.SetTranslation(FVector(0, -yWidth/2, height / 2));
		wallTransform.SetScale3D(FVector(1, xWidth / 256, height / 256));
		wall->spawnWall(wallTransform);
		wall->applyDemoWallParams();

		//+y
		wall = GetWorld()->SpawnActor<AGWall>(spawnLocation, FRotator::ZeroRotator, spawnParams);
		wallTransform.SetRotation(FQuat(FRotator(0, 90, 0)));
		wallTransform.SetTranslation(FVector(0, yWidth / 2, height / 2));
		wallTransform.SetScale3D(FVector(1, xWidth / 256, height / 256));
		wall->spawnWall(wallTransform);
		wall->applyDemoWallParams();

	}

	//+z spawn ceiling 
	if (autoSpawnCeiling) {
		FVector spawnLocation = FVector(0, 0, 0);
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AGWall* wall = GetWorld()->SpawnActor<AGWall>(spawnLocation, FRotator::ZeroRotator, spawnParams);
		FTransform wallTransform;
		wallTransform.SetRotation(FQuat(FRotator(90, 0, 0)));
		wallTransform.SetTranslation(FVector(0, 0, height));
		wallTransform.SetScale3D(FVector(1, xWidth / 256, yWidth / 256));

		wall->spawnWall(wallTransform);
		wall->applyDemoWallParams();
	}
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

	json::value_type x = actor["xCenter"];
	if (x.is_number()) {
		relX = x;
	}
	json::value_type y = actor["yCenter"];
	if (y.is_number()) {
		relY = y;
	}
	json::value_type z = actor["zCenter"];
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

	//FVector spawnLocation = FVector(origin.X + relX, origin.Y + relY, origin.Z + relZ) * FVector(-1.0, 1.0, 1.0);
	FVector spawnLocation = FVector(origin.X + relX, origin.Y + relY, origin.Z + relZ) * FVector(-1.0, 1.0, 1.0);
	if (!actor["type"].is_string()) {
		return;
	}

	if (actor["name"].is_string()) {
		FString displayName = FString(actor["name"].get<json::string_t>().c_str());
		UE_LOG(LogTemp, Warning, TEXT("Spawning %s at (%f %f, %f)"), *displayName, spawnLocation.X, spawnLocation.Y, spawnLocation.Z);
	}

	
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	FString type = FString(actor["type"].get<json::string_t>().c_str());
	bool success = false;
	AGymObj* spawnedObj = nullptr;
	if (actor["actorParams"].is_object()) {
		json::object_t& actorParams = actor["actorParams"].get_ref<json::object_t&>();
		if (type.ToLower() == "GymObj") {
			AGymObj* spawnedGymObj = GetWorld()->SpawnActor<AGymObj>(spawnLocation, FRotator::ZeroRotator, spawnParams);
			if (actorParams["meshPath"].is_string()) {
				FString path = FString(actorParams["meshPath"].get<json::string_t>().c_str());
				success = spawnedGymObj->assignMeshFromPath(path, spawnLocation, actorParams);
			}
			spawnedObj = spawnedGymObj;
			UE_LOG(LogTemp, Warning, TEXT("Tried GymObj"))
		}
		else if (type.ToLower() == "GShapenet") {
			AGShapenet* spawnedGymShapenetObj = GetWorld()->SpawnActor<AGShapenet>(spawnLocation, FRotator::ZeroRotator, spawnParams);
			if (actorParams["shapenetSynset"].is_string() && actorParams["shapenetHash"].is_string()) {
				FString synset = FString(actorParams["shapenetSynset"].get<json::string_t>().c_str());
				FString id = FString(actorParams["shapenetHash"].get<json::string_t>().c_str());
				success = spawnedGymShapenetObj->assignMeshFromSynsetAndModelID(synset, id, spawnLocation, actorParams);
			}
			else if (actorParams["shapenetSynset"].is_string()) {
				FString synset = FString(actorParams["shapenetSynset"].get<json::string_t>().c_str());
				success = spawnedGymShapenetObj->assignRandomFromSynset(synset, spawnLocation, actorParams);
			}
			spawnedObj = spawnedGymShapenetObj;
		}
		else if (type.ToLower() == "GPartnet") {
			AGPartnet* spawnedGymPartnetObj = GetWorld()->SpawnActor<AGPartnet>(spawnLocation, FRotator::ZeroRotator, spawnParams);

			if (actorParams["category"].is_string() && actorParams["annotatrioID"].is_string()) {
				FString category = FString(actorParams["category"].get<json::string_t>().c_str());
				FString annotationId = FString(actorParams["annotationId"].get<json::string_t>().c_str());
				success = spawnedGymPartnetObj->assignMeshFromCategoryAndAnnotationID(category, annotationId, spawnLocation, actorParams);
			}
			else if (actorParams["category"].is_string()) {
				FString category = FString(actorParams["category"].get<json::string_t>().c_str());
				success = spawnedGymPartnetObj->assignRandomMeshFromCategory(category, spawnLocation, actorParams);
			}
			spawnedObj = spawnedGymPartnetObj;

		}
		else if (type.ToLower() == "GLighting") {
			spawnedObj = GetWorld()->SpawnActor<AGLighting>(spawnLocation, FRotator::ZeroRotator, spawnParams);
			// TODO: implement GLighting
		}
		else if (type.ToLower() == "GWall") {
			spawnedObj = GetWorld()->SpawnActor<AGWall>(spawnLocation, FRotator::ZeroRotator, spawnParams);
			// TODO: implement GWall
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("GymObj type %s not supported"), *type)
				return;
		}
	}

	if (success && spawnedObj) {
		gymObjects.Add(spawnedObj);
		if (actor["name"].is_string()) {
			FString displayName = FString(actor["name"].get<json::string_t>().c_str());
			spawnedObj->SetActorLabel(displayName);
			if (displayName == "heatSource") {
				heatSource = spawnedObj;
				heatSource->addFire("indoor");
			}
			else if (displayName == "salt") {
				salt = spawnedObj;
			}
			else if (displayName == "meat") {
				meat = spawnedObj;
			}
			else if (displayName == "pan") {
				pan = spawnedObj;
			}
			else if (displayName == "walnut") {
				walnut = spawnedObj;
				spawnedObj->setMaterial("/Game/DemoAssets/Walnut/newwalnutmat.newwalnutmat");
			}
			else if (displayName == "brick") {
				//brick = spawnedObj;
				//spawnedObj->setMaterial("/Game/Kitchen/Material/Wood_Cherry_Original.Wood_Cherry_Original");
			}
			else if (displayName == "hammer") {
				hammer = spawnedObj;
			} 
			else if (displayName == "banana") {
				//banana = spawnedObj;
				spawnedObj->setMaterial("/Game/Scenes/Small_Items/Distractor/sponge/Yellow.Yellow");
			}
			else if (displayName == "candyBar") {
				//candyBar = spawnedObj;
			}
			else if (displayName == "wrench") {
				//wrench = spawnedObj;
			}
			else if (displayName == "waterBottle") {
				//waterBottle = spawnedObj;
			}
			else if (displayName == "cereal") {
				//cereal = spawnedObj;
			}
			else if (displayName == "outside" || displayName == "outside2") {
				spawnedObj->SetActorScale3D(FVector(0.6, 0.6, 0.9));
			}
			else if (displayName == "knife") {
				knife = spawnedObj;
			}
			else if (displayName == "book") {
				book = spawnedObj;
			}
			else if (displayName == "grater") {
				grater = spawnedObj;
			}
			else if (displayName == "stone") {
				stone = spawnedObj;
			}
			
		}
		UE_LOG(LogTemp, Warning, TEXT("Spawn Success"))
	}
	else {
		if (spawnedObj) {
			spawnedObj->Destroy();
		}
		UE_LOG(LogTemp, Warning, TEXT("Spawn Failure"))
	}

}


void ASceneGenUnrealGameModeBase::resetDamping()
{
	for (int32 i = 0; i < gymObjects.Num(); i++) {
		if (gymObjects[i]) {
			if (gymObjects[i]->getBaseMesh()) {
				gymObjects[i]->getBaseMesh()->SetLinearDamping(0.01);
				gymObjects[i]->getBaseMesh()->SetAngularDamping(0.01);
			}
		}
	}
	objectsDamped = false;
	UE_LOG(LogTemp, Warning, TEXT("Ready to run episode"));
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

		if (dstParams.find("gravityEnabled") == dstParams.end() && srcParams.find("gravityEnabled") != srcParams.end()) {
			dstParams["gravityEnabled"] = srcParams["gravityEnabled"];
		}

		if (dstParams.find("globalScale") == dstParams.end() && srcParams.find("globalScale") != srcParams.end()) {
			dstParams["globalScale"] = srcParams["globalScale"];
		}

		if (dstParams.find("useRandomTextures") == dstParams.end() && srcParams.find("useRandomTextures") != srcParams.end()) {
			dstParams["useRandomTextures"] = srcParams["useRandomTextures"];
		}

		if (dstParams.find("canOverlap") == dstParams.end() && srcParams.find("canOverlap") != srcParams.end()) {
			dstParams["canOverlap"] = srcParams["canOverlap"];
		}
		if (dstParams.find("worldScale") == dstParams.end() && srcParams.find("worldScale") != srcParams.end()) {
			dstParams["worldScale"] = srcParams["worldScale"];
		}

		if (dstParams.find("pitch") == dstParams.end() && srcParams.find("pitch") != srcParams.end()) {
			dstParams["pitch"] = srcParams["pitch"];
		}
		
		if (dstParams.find("yaw") == dstParams.end() && srcParams.find("yaw") != srcParams.end()) {
			dstParams["yaw"] = srcParams["yaw"];
		}

		if (dstParams.find("roll") == dstParams.end() && srcParams.find("roll") != srcParams.end()) {
			dstParams["roll"] = srcParams["roll"];
		}

		if (dstParams.find("meshPath") == dstParams.end() && srcParams.find("meshPath") != srcParams.end()) {
			dstParams["meshPath"] = srcParams["meshPath"];
		}


		if (dstParams.find("autoHeight") == dstParams.end() && srcParams.find("autoHeight") != srcParams.end()) {
			dstParams["autoHeight"] = srcParams["autoHeight"];
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


bool ASceneGenUnrealGameModeBase::interact(AGymAgent* agent, AGymObj* obj)
{
	// no interaction by default
	return false;
}

bool ASceneGenUnrealGameModeBase::interact(AGymObj* obj, AGymAgent* agent)
{
	// no interaction by default
	return false;
}

bool ASceneGenUnrealGameModeBase::interact(AGymAgent* agent1, AGymAgent* agent2)
{
	// no interaction by default
	return false;
}

bool ASceneGenUnrealGameModeBase::interact(AGymObj* obj1, AGymObj* obj2)
{
	// no interaction by default
	return false;
}


TArray<AGymObj*> ASceneGenUnrealGameModeBase::getGymObjects()
{
	return gymObjects;
}

TArray<AGymObj*> ASceneGenUnrealGameModeBase::getInteractiveGymObjects()
{
	TArray<AGymObj*> interactiveGymObjects;
	for (int32 i = 0; i < gymObjects.Num(); i++) {
		if (gymObjects[i]->getInteractsWithGymObjs() || gymObjects[i]->getInteractsWithGymAgents()) {
			interactiveGymObjects.Add(gymObjects[i]);
		}
	}
	return interactiveGymObjects;
}

TArray<AGymObj*> ASceneGenUnrealGameModeBase::getAgentInteractiveGymObjects()
{
	TArray<AGymObj*> interactiveGymObjects;
	for (int32 i = 0; i < gymObjects.Num(); i++) {
		if (gymObjects[i]->getInteractsWithGymAgents()) {
			interactiveGymObjects.Add(gymObjects[i]);
		}
	}
	return interactiveGymObjects;
}
TArray<AGymObj*> ASceneGenUnrealGameModeBase::getObjectInteractiveGymObjects()
{
	TArray<AGymObj*> interactiveGymObjects;
	for (int32 i = 0; i < gymObjects.Num(); i++) {
		if (gymObjects[i]->getInteractsWithGymObjs()) {
			interactiveGymObjects.Add(gymObjects[i]);
		}
	}
	return interactiveGymObjects;
}

TArray<AGymAgent*> ASceneGenUnrealGameModeBase::getGymAgents()
{
	return gymAgents;
}

void ASceneGenUnrealGameModeBase::distributeReward(AGymAgent* agent)
{
	return;
}

FString ASceneGenUnrealGameModeBase::getMode()
{
	return mode;
}

void ASceneGenUnrealGameModeBase::setMode(FString newMode)
{
	mode = newMode;
}

void ASceneGenUnrealGameModeBase::attachToAgent(AGDemoAgent* agent)
{
	FVector socketLocation = agent->baseMesh->GetSocketLocation("hand_rSocket");

	float distances[15] = {-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,-1,-1, -1};

	if (meat) {
		distances[0] = FVector::Dist(socketLocation, meat->GetActorLocation());
	}

	if (salt) {
		distances[1] = FVector::Dist(socketLocation, salt->GetActorLocation());
	}

	if (lighter) {
		distances[2] = FVector::Dist(socketLocation, lighter->GetActorLocation());
	}

	if (pan) {
		distances[3] = FVector::Dist(socketLocation, pan->GetActorLocation());
	}

	if (knife) {
		distances[4] = FVector::Dist(socketLocation, knife->GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("knife check"))
	}

	if (hammer) {
		distances[5] = FVector::Dist(socketLocation, hammer->GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("hammer check"))
	}

	if (brick) {
		distances[6] = FVector::Dist(socketLocation, brick->GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("brick check"))

	}

	// new stuff
	if (wrench) {
		distances[7] = FVector::Dist(socketLocation, wrench->GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("WRENCH check"))
	}

	if (waterBottle) {
		distances[8] = FVector::Dist(socketLocation, waterBottle->GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("WbOTTLE check"))

	}

	if (banana) {
		distances[9] = FVector::Dist(socketLocation, banana->GetActorLocation());
	}

	if (candyBar) {
		distances[10] = FVector::Dist(socketLocation, candyBar->GetActorLocation());
	}

	if (cereal) {
		distances[11] = FVector::Dist(socketLocation, cereal->GetActorLocation());
	}

	if (stone) {
		distances[12] = FVector::Dist(socketLocation, stone->GetActorLocation());
	}

	if (book) {
		distances[13] = FVector::Dist(socketLocation, book->GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("book check"))
	}


	if (grater) {
		distances[14] = FVector::Dist(socketLocation, grater->GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("grater check"))
	}




	float min = -1;
	int index = 0;
	for (int32 i = 0; i < 15; i++) {
		if (min == -1 || (distances[i] < min && distances[i] != -1)) {
			min = distances[i];
			index = i;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Min index : %d"), index)

	AGymObj* closest = nullptr;
	switch (index) {
		case 0: closest = meat; break;
		case 1: closest = salt; break;
		case 2: closest = lighter; break;
		case 3: closest = pan; break;
		case 4: closest = knife; break;
		case 5: closest = hammer; break;
		case 6: closest = brick; break;
		case 7: closest = wrench; break;
		case 8: closest = waterBottle; break;
		case 9: closest = banana; break;
		case 10: closest = candyBar; break;
		case 11: closest = cereal; break;
		case 12: closest = stone; break;
		case 13: closest = book; break;
		case 14: closest = grater; break;
	}
	
	if (closest) {
		FTimerDelegate TimerDel;
		FTimerHandle TimerHandle;
		UE_LOG(LogTemp, Warning, TEXT("Closeset: %s "), *closest->GetName())

		TimerDel.BindUFunction(this, FName("asyncAttach"), agent, closest, index);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 0.760f, false);
	}
}

void ASceneGenUnrealGameModeBase::asyncAttach(AGDemoAgent* agent, AGymObj* closest, int index)
{
	closest->SetActorEnableCollision(false);
	closest->getBaseMesh()->SetSimulatePhysics(false);
	closest->AttachToComponent(agent->baseMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true), FName("hand_rSocket"));
	switch (index) {
		/*
		case 0: closest->SetActorRelativeLocation(FVector(-15, 5, 0)); closest->SetActorRelativeRotation(FRotator(45,-20,0)); break;
		case 1: closest->SetActorRelativeLocation(FVector(-12, 9, -5)); closest->SetActorRelativeRotation(FRotator(0,0,90)); break;
		case 2: closest->SetActorRelativeLocation(FVector(-10, 5, -10)); break;
		*/
		case 0: closest->SetActorRelativeLocation(FVector(0, 8, 0)); closest->SetActorRelativeRotation(FRotator(-30, 0, -100)); break;
		case 1: closest->SetActorRelativeLocation(FVector(5, 0, 0)); closest->SetActorRelativeRotation(FRotator(0, -120, 35)); break;
		case 2: closest->SetActorRelativeLocation(FVector(3, -5, -4)); closest->SetActorRelativeRotation(FRotator(45, 0, 45)); break;
		case 3: closest->SetActorRelativeLocation(FVector(-5, 4, 5)); closest->SetActorRelativeRotation(FRotator(-36.08, -42.12, -43.12)); break;
		case 4: //closest->SetActorRelativeLocation(FVector(2, 25, 8)); closest->SetActorRelativeRotation(FRotator(30.64, 81.62, -92.7)); break;
				closest->SetActorRelativeLocation(FVector(-8, 18, 2)); closest->SetActorRelativeRotation(FRotator(8, 101.2, -140.8)); break;
		case 5: closest->SetActorRelativeLocation(FVector(-9.1, 6, 2)); closest->SetActorRelativeRotation(FRotator(24.3, -125.4, 107.9)); break;
		case 6: closest->SetActorRelativeLocation(FVector(7, 2, -2)); closest->SetActorRelativeRotation(FRotator(45.6, 25.1, 43.7)); break;
		case 7: break;
		case 8: closest->SetActorRelativeLocation(FVector(15, -4.4, -6.2)); closest->SetActorRelativeRotation(FRotator(-46.6, 97.2, 39.9)); break;
		case 9: closest->SetActorRelativeLocation(FVector(4, 1, -2)); closest->SetActorRelativeRotation(FRotator(-6.7, -115.4, 22.6)); break;
		case 10: closest->SetActorRelativeLocation(FVector(3, -7, -8)); closest->SetActorRelativeRotation(FRotator(-32, -36.5, 88.5)); break;
		case 11: closest->SetActorRelativeLocation(FVector(33.7, 30.0, -24.2)); closest->SetActorRelativeRotation(FRotator(27.5, -119.8, 30.3)); break;
		case 12: closest->SetActorRelativeLocation(FVector(0, 3, 0)); closest->SetActorRelativeRotation(FRotator(-27.2, -13.5, -6.4)); break;
		case 13:  closest->SetActorRelativeLocation(FVector(-1.3, 5.2, 5.8)); closest->SetActorRelativeRotation(FRotator(4.2, -119.9, 39)); break;
		case 14:  closest->SetActorRelativeLocation(FVector(0, 0, 2)); closest->SetActorRelativeRotation(FRotator(55.8, -25.6, -3)); break;
	}
	agent->setHeldObject(closest);
	UE_LOG(LogTemp, Warning, TEXT("Attached"))
}

void ASceneGenUnrealGameModeBase::dispenseSalt()
{
	if (salt) {
		salt->dispenseSalt();
	}
}

void ASceneGenUnrealGameModeBase::cook()
{
	if (meat) {
		meat->setMaterial("/Game/Assets/Meat/cookedMat.cookedMat");
	}
}

void ASceneGenUnrealGameModeBase::crack()
{
	if (walnut) {
		walnut->setMaterial("/Game/Assets/Meat/cookedMat.cookedMat");
	}
}