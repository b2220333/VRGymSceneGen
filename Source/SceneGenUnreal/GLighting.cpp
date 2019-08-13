// Fill out your copyright notice in the Description page of Project Settings.

#include "GLighting.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Components/PointLightComponent.h"
#include "Runtime/Engine/Classes/Components/DirectionalLightComponent.h"


#include "Runtime/Engine/Classes/Engine/DirectionalLight.h" 
#include "Runtime/Engine/Classes/Engine/PointLight.h"

bool AGLighting::spawnPointLight(FVector location)
{
	UPointLightComponent* pointLightComponent = NewObject<UPointLightComponent>(this, UPointLightComponent::StaticClass());
	if (pointLightComponent) {
		RootComponent = pointLightComponent;
		RootComponent->SetWorldLocation(location);
		light = pointLightComponent;
		lightType = "point";
		return true;
	}
	return false;
}

bool AGLighting::spawnDirectionalLight(FVector location, FRotator rotation)
{
	UDirectionalLightComponent* directionLightComponent = NewObject<UDirectionalLightComponent>(this, UDirectionalLightComponent::StaticClass());
	if (directionLightComponent) {
		RootComponent = directionLightComponent;
		RootComponent->SetWorldLocation(location);
		RootComponent->SetWorldRotation(rotation);
		light = directionLightComponent;
		lightType = "directional";
		return true;
	}
	return false;
}

FString AGLighting::getLightType()
{
	return lightType;
}

ULightComponent* AGLighting::getLight()
{
	return light;
}

