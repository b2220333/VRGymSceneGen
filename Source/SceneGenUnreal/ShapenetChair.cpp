// Fill out your copyright notice in the Description page of Project Settings.

#include "ShapenetChair.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"



// Sets default values
AShapenetChair::AShapenetChair()
{
	// unreal engine calls parent class constructor automatically before anything that happens below 

}

// Called when the game starts or when spawned
void AShapenetChair::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AShapenetChair::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShapenetChair::importModel(FString annotationID)
{
	json::object_t metaJson;
	FString path = partnetDir + "/" + annotationID + "/meta.json";
	FString metaJsonStr;
	FFileHelper::LoadFileToString(metaJsonStr, *path);
	json parsed = json::parse(std::string(TCHAR_TO_UTF8(*metaJsonStr)));

	if (parsed.is_object()) {
		metaJson = parsed.get<json::object_t>();
	}
	else {
		return;
	}
	

}

