#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "UnrealEd.h"
#include "Runtime/Core/Public/CoreGlobals.h"

#include "Runtime/CoreUObject/Public/UObject/Class.h"


#include "Shapenet.h"


DECLARE_LOG_CATEGORY_EXTERN(ShapenetImportModule, All, All)

class FShapenetImportModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	bool importOBJ(FString synset, FString hash);
	FString shapenetDir = "C://Users/jamqd/data/ShapeNetCore.v2";
	bool modelAlreadyImported(FString synset, FString hash);

	struct SearchResult{
		FString synset;
		uint64 numModels;
	};
	

	SearchResult searchShapenet(FString query);

};
