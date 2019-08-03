// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GymObj.generated.h"

UCLASS()
class SCENEGENUNREAL_API AGymObj : public AActor
{

	/*
		Abstract class for objects in VRGym

		GymObj (Only for objects not in Shapenet or Partnet)
		|
		|___GShapenetObj
		|	|___SNChair
		|	|___SNDishwasher
		|	|
		|	.
		|	.
		|	.
		|
		|___GPartnetObj (No animations / interaction with specific partnet parts)
			|___PNChair
			|___PNDishwasher
			|
			.
			.
			.


	
	*/


	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGymObj();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
