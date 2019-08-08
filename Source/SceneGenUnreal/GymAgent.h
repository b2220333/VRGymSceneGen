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
		Parent class for all agents

		GymAgent (Only for objects not in Shapenet, Partnet, Wall, or Lighting)
		|
		|___GDemoAgent (For Muri Demo)
		|
		|___GRobotAgent
		|	|___GRBaxterAgent
		|	|
		|	.
		|	.
		|	.
		|
		|___GVehicleAgent
		|	|___GCarAgent
		|	|___GPlaneAgent
		|	|___GHelicopterAgent
		|	|
		|	.
		|	.
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

	// moves forward when W or up arrow pressed, backward when S or down arrow pressed
	void MoveForward(float Value);

	// moves right when D or right arrow pressed, left when A or left arrow pressed
	void MoveRight(float Value);

	// interacts with GymObj if nearby and facing when spacebar pressed
	void interact();


	// sets mesh of agent
	void setMesh(FString path);

	UMeshComponent* getMesh();

private:

	UPROPERTY(EditAnywhere)
	UMeshComponent* baseMesh;
	
};

