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
#include "GymObj.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Runtime/Engine/Classes/GameFramework/Controller.h"
#include "GDemoAgentMovementComponent.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "SceneGenUnrealGameModeBase.h"

AGDemoAgent::AGDemoAgent()
{
	PrimaryActorTick.bCanEverTick = true;
	
	UCapsuleComponent* capsule = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	capsule->InitCapsuleSize(42.f, 96.0f);
	RootComponent = capsule;

	baseMesh = CreateDefaultSubobject <USkeletalMeshComponent>("baseMesh");
	FString path = "/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin";
	USkeletalMesh* test2 = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, *path));

	baseMesh->SetSkeletalMesh(test2);
	baseMesh->SetMobility(EComponentMobility::Movable);


	FVector agentLocation = GetActorLocation();
	baseMesh->SetWorldLocation(agentLocation - FVector(0, 0, 140));
	baseMesh->SetRelativeRotation(FVector(0, -90, 0).ToOrientationQuat());
	baseMesh->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	

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
	
	/*
	CameraBoom->SetWorldLocation(agentLocation + FVector(0, 0, 140));
	CameraBoom->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	*/
	CameraBoom->TargetArmLength = 300.0f; // The camera  follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	movementComponent = CreateDefaultSubobject<UGDemoAgentMovementComponent>(TEXT("MovementComponent"));
	movementComponent->UpdatedComponent = RootComponent;
	isHoldingObject = false;
	walking = false;
}

void AGDemoAgent::BeginPlay()
{
	Super::BeginPlay();
	
	/*
	FString path = "AnimSequence'/Game/Mannequin/Animations/ThirdPersonRun.ThirdPersonRun'B";
	//UAnimationAsset* animation = Cast<UAnimationAsset>(StaticLoadObject(UAnimationAsset::StaticClass(), nullptr, *path));
	animation = LoadObject<UAnimationAsset>(nullptr, *path);
	*/
	playAnimation("ThirdPersonIdle", true);


}

void AGDemoAgent::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	
	UE_LOG(LogTemp, Warning, TEXT("Binding success"))
	// interaction 
	PlayerInputComponent->BindAction("Pickup", IE_Pressed, this, &AGDemoAgent::pickUpObject);
	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &AGDemoAgent::dropObject);
	PlayerInputComponent->BindAction("Shake", IE_Pressed, this, &AGDemoAgent::shakeSalt);

	// from third person
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

void AGDemoAgent::pickUpObject()
{
	if (isHoldingObject) {
		return;
	}
	playAnimation("ThirdPersonPickup", false);
	ASceneGenUnrealGameModeBase* mode = (ASceneGenUnrealGameModeBase*)GetWorld()->GetAuthGameMode();
	mode->attachToAgent(this);
	isHoldingObject = true;
}

void AGDemoAgent::dropObject()
{
	if (!isHoldingObject) {
		return;
	}
	playAnimation("ThirdPersonDrop", false);
	heldObject->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
	heldObject->getBaseMesh()->SetSimulatePhysics(true);
	heldObject->SetActorEnableCollision(true);

	isHoldingObject = false;
}

void AGDemoAgent::shakeSalt()
{
	/*
	if (!heldObject || !isHoldingObject || heldObject->getName() != "salt") {
		return;
	}
	*/
	
	playAnimation("shake", false);

}



// function below are  from third person starter content

void AGDemoAgent::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AGDemoAgent::TurnAtRate(float AxisValue)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += AxisValue;
	SetActorRotation(NewRotation);
}

void AGDemoAgent::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AGDemoAgent::MoveForward(float AxisValue)
{
	if (movementComponent && (movementComponent->UpdatedComponent == RootComponent))
	{
		movementComponent->AddInputVector(GetActorForwardVector() * AxisValue);
	}
}


void AGDemoAgent::MoveRight(float AxisValue)
{
	if (movementComponent && (movementComponent->UpdatedComponent == RootComponent))
	{
		movementComponent->AddInputVector(GetActorRightVector() * AxisValue);
	}
}

void AGDemoAgent::playRandomAnimation()
{
	TArray<FString> animations = {
		"ThirdPerson_Jump", "ThirdPersonDrop", "ThirdPersonIdle", "ThirdPersonJump_End","ThirdPersonJump_Loop", 
		"ThirdPersonJump_Start", "ThirdPersonPickup", "ThirdPersonRun", "ThirdPersonWalk"
	};

	int32 i = FMath::RandRange(0, animations.Num() - 1);
	playAnimation(animations[i], true);


}


void AGDemoAgent::playAnimation(FString animName, bool looping)
{
	UE_LOG(LogTemp, Warning, TEXT("Trying to play animation : %s"), *animName)
	
	FString path = "AnimSequence'/Game/Mannequin/Animations/" + animName + "." + animName + "'B";
	animation = LoadObject<UAnimationAsset>(nullptr, *path);
	if (animation && baseMesh) {
		baseMesh->PlayAnimation(animation, looping);
	}
}

void AGDemoAgent::Tick(float DeltaTime)
{
	FVector velocity = movementComponent->Velocity;
		if (velocity.Size() > 0) {
			if (!walking) {
				walking = true;
				playAnimation("ThirdPersonWalk", true);
			}
		}
		else {
			if (walking) {
				walking = false;
				playAnimation("ThirdPersonIdle", true);
			}
		}
}

UPawnMovementComponent* AGDemoAgent::GetMovementComponent() const
{
	return movementComponent;
}

void AGDemoAgent::setHeldObject(AGymObj* obj)
{
	heldObject = obj;
}