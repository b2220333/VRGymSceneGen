//Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GymAgent.h"
#include "GDemoAgent.generated.h"

class AGymObject;

UCLASS()
class SCENEGENUNREAL_API AGDemoAgent : public AGymAgent
{
	GENERATED_BODY()
	
	/*
	For MURI demo. Will use Unreal Engine default third person hero skeletal mesh.
	*/

public:
	//constructor
	AGDemoAgent();

	virtual void BeginPlay() override;

	// tries to pickup a gym object e.g. pots, pans, ingredients
	void pickUpObject();

	// drops a object if it is holding one
	void dropObject();

	// releases an object with an initial velocity forward
	void throwObject();
	
	// must be public for animation to work for some reason
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* baseMesh;
	UAnimationAsset *animation;


	void playRandomAnimation();

	void playAnimation(FString animationAssetPath, bool looping);

	virtual void Tick(float DeltaTime) override;


	class UGDemoAgentMovementComponent* movementComponent;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

private:
	// whether the agent is holding an object
	bool isHoldingObject;

	// the object that the agent is holding
	AGymObject* heldObject;
	
	

	// from  third person starter content
private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
public:
	

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	
	
};
