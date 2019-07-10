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
	FString shapenetDir = "D://data/ShapeNetCore.v2";
	
	bool modelAlreadyImported(FString synset, FString hash);

	struct SearchResult{
		TArray<FString> synsets;
		TArray<int64> numModels;
	};
	

	SearchResult searchShapenet(FString query);
	bool synsetExists(FString query);
	bool importSynset(FString synset);
	
	/*
	
	srcPath is absolute path to model file
	dstPath is relative to game and must be prefixed by /Gane/ which 
	is unreal's way of accessing the content folder of the project
	
	*/
	bool importOBJFromFile(FString srcPath, FString dstPath);

	void setShapenetDir(FString path);
	void FString getShapenetDir();

	bool importAllShapenetModels();

	// menu exposed commands

	TSharedPtr<FUICommandInfo> importFromPath;
	TSharedPtr<FUICommandInfo> importFromSynsetAndHash;
	TSharedPtr<FUICommandInfo> importSynset;
	TSharedPtr<FUICommandInfo> importAllShapenet;
	TSharedPtr<FUICommandInfo> changeShapenetDir;

};
