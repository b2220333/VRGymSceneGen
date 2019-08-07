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

	// interaction with a gym agent
	void interactWith(class AGymAgent* agent);

	// interaction with a gym obj
	void interactWith(class GymObj* obj);

private:

	UPROPERTY(EditAnywhere)
	UMeshComponent* baseMesh;
	
};

