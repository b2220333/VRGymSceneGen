#pragma once


#include "json.hpp"
using json = nlohmann::json;


#include "CoreMinimal.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "UnrealEd.h"
#include "Runtime/Core/Public/CoreGlobals.h"

#include "Runtime/CoreUObject/Public/UObject/Class.h"




DECLARE_LOG_CATEGORY_EXTERN(ShapenetImportModule, All, All)


class FShapenetImportModule : public IModuleInterface
{
private:
	FString shapenetDir = "D://data/ShapeNetCore.v2";
	FString partnetDir = "D://data/data_v0"; //clone partnet github repo into this directory
	TSharedPtr<class FUICommandList> ModuleCommands;
	

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


	bool importFromSynsetAndHash(FString synset, FString hash);
	
	bool modelAlreadyImported(FString synset, FString hash);

	struct SearchResult{
		TArray<FString> synsets;
		TArray<int64> numModels;
	};
	

	SearchResult searchShapenet(FString query);
	bool synsetExists(FString query);
	bool importSynset(FString synset, int32 numToImport);
	
	/*
	
	srcPath is absolute path to model file
	dstPath is relative to game and must be prefixed by /Gane/ which 
	is unreal's way of accessing the content folder of the project
	
	*/
	bool importFromFile(FString srcPath, FString dstPath);
	bool importFromDir(FString srcPath, FString dstPath);

	void setShapenetDir(FString path);
	FString getShapenetDir();


	// button clicked test
	void onImportButtonClicked();

	void AddToolbarExtension(FToolBarBuilder& Builder);
	void AddMenuExtension(FMenuBuilder& Builder);

	bool importFromJson(FString jsonString);

	FDateTime importJsonLastChanged;
	 


	void importPartnetFromAnnotationID(FString annotationID);
	void tryImportPartnetQuery(FString query, int32 numToImport);
	json::object_t getPartnetMetaJson(FString annotationID);
	TArray<FString> searchPartnet(FString query);
	

};
