// Fill out your copyright notice in the Description page of Project Settings.

#include "GymAgent.h"
#include "SceneGenUnrealGameModeBase.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"

// Sets default values
AGymAgent::AGymAgent()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGymAgent::BeginPlay()
{
	
	Super::BeginPlay();
	/*
	FString path = "AnimSequence'/Game/Mannequin/Animations/ThirdPersonWalk.ThirdPersonWalk'B";
	animation = Cast<UAnimationAsset>(StaticLoadObject(UAnimationAsset::StaticClass(), nullptr, *path));
	if(testSwitch) {
		test->PlayAnimation(animation, true);
	}
	*/
}

// Called every frame
void AGymAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGymAgent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("interact", IE_Pressed, this, &AGymAgent::interact);
}


void AGymAgent::interact()
{
	ASceneGenUnrealGameModeBase* gameMode = GetWorld()->GetAuthGameMode<ASceneGenUnrealGameModeBase>();
	TArray<AGymObj*> gymObjects = gameMode->getAgentInteractiveGymObjects();
	
	// pickup object if close enough
	for (int32 i = 0; i < gymObjects.Num(); i++) {
		FVector objLocation = gymObjects[i]->GetActorLocation();
		if (FVector::Dist(objLocation, GetActorLocation()) < 50) {
			// game mode handles interaction mechanics
			gameMode->interact(this, gymObjects[i]);
		}
	}

}
