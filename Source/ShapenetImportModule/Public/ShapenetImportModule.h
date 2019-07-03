#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "UnrealEd.h"
#include "Runtime/Core/Public/CoreGlobals.h"

DECLARE_LOG_CATEGORY_EXTERN(ShapenetImportModule, All, All)

class FShapenetImportModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};