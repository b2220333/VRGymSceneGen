// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Runtime/Engine/Classes/Components/MeshComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GymAgent.generated.h"

UCLASS()
class SCENEGENUNREAL_API AGymAgent : public APawn
{
	GENERATED_BODY()

	/*
		GymAgent (Parent class for all agents)
		|
		|___GDemoAgent (For Muri Demo)
		|
		|___GPhysicalAgent (Agents with physical properties e.g. movement interaction with other physical actors
		|	|___GRobotAgent
		|	|	|___GRBaxterAgent
		|	|	|
		|	|	.
		|	|	.
		|	|	.
		|	|
		|	|___GVehicleAgent
		|	|	|___GCarAgent
		|	|	|___GPlaneAgent
		|	|	|___GHelicopterAgent
		|	|	|
		|	|	.
		|	.	.
		|	.	.
		|	.
		.
		.
		.


	*/
public:
	// Sets default values for this pawn's properties
	AGymAgent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	

	// interacts with GymObj if nearby and facing when spacebar pressed
	void interact();

private:
	// total reward accumulated in current episode
	float totalCurrentReward;

	// previous rewards of current episode
	TArray<float> rewardsFromThisEpisode;

	// reward history of previous episodes (all time steps)
	TArray<TArray<float>> rewardHistory;

	// reward history of previous episodes (accumulated reward)
	TArray<float> rewardHistoryAccumulated;

		
};

