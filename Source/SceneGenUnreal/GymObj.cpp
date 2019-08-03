// Fill out your copyright notice in the Description page of Project Settings.

#include "GymObj.h"


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

