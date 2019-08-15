// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GDemoAgentMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SCENEGENUNREAL_API UGDemoAgentMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	
	
};
