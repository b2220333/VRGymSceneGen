// Fill out your copyright notice in the Description page of Project Settings.

#include "GDemoAgent.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Animation/AnimBlueprint.h"
#include "Runtime/Engine/Classes/Animation/AnimSequence.h"
#include "Runtime/HeadMountedDisplay/Public/HeadMountedDisplayFunctionLibrary.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "Runtime/Engine/Classes/Animation/AnimSingleNodeInstance.h"
#include "Runtime/Engine/Classes/Animation/AnimationAsset.h"

AGDemoAgent::AGDemoAgent()
{
	/*
	baseMesh = CreateDefaultSubobject <USkeletalMeshComponent>("baseMesh");
	FString path = "/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin";
	USkeletalMesh* test = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, *path));

	baseMesh->SetSkeletalMesh(test);
	
	RootComponent = baseMesh;

	
	


	// from third person starter content

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera  follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	*/
	
	testSwitch = false;
}

void AGDemoAgent::BeginPlay()
{
	//Super::BeginPlay();
	
	FString path = "AnimSequence'/Game/Mannequin/Animations/ThirdPersonWalk.ThirdPersonWalk'B";
	//UAnimationAsset* animation = Cast<UAnimationAsset>(StaticLoadObject(UAnimationAsset::StaticClass(), nullptr, *path));
	animation = LoadObject<UAnimationAsset>(nullptr, *path);
	
	test->PlayAnimation(animation, true);
	
	/*
	FString path = "AnimSequence'/Game/Mannequin/Animations/ThirdPersonRun.ThirdPersonRun'B";
	animation = Cast<UAnimationAsset>(StaticLoadObject(UAnimationAsset::StaticClass(), nullptr, *path));
	if (!animation) {
		UE_LOG(LogTemp, Warning, TEXT("Could not find animation %s"), *path)
		return;
	}
	
	
	//baseMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	//baseMesh->AnimScriptInstance = &animObj.Object;
	UE_LOG(LogTemp, Warning, TEXT("Animation set"))
		

	//bool test = baseMesh->InitializeAnimScriptInstance(true);
	//baseMesh->AnimScriptInstance = NewObject<UAnimSingleNodeInstance>(baseMesh, UAnimSingleNodeInstance::StaticClass());
	/*
	if (!test) {
		UE_LOG(LogTemp, Warning, TEXT("Failed initialization"))
		if (!baseMesh->AnimScriptInstance) {
			UE_LOG(LogTemp, Warning, TEXT("Actually Failed initialization"))
		}
	}*/
	
	/*
	//baseMesh->SetAnimation(animation);
	baseMesh->PlayAnimation(animation, true);
	//baseMesh->Play(true);
	*/
	



}

void AGDemoAgent::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	

	PlayerInputComponent->BindAxis("MoveForward", this, &AGDemoAgent::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGDemoAgent::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AGDemoAgent::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AGDemoAgent::LookUpAtRate);



	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AGDemoAgent::OnResetVR);
}


void AGDemoAgent::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}



void AGDemoAgent::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AGDemoAgent::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AGDemoAgent::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AGDemoAgent::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}