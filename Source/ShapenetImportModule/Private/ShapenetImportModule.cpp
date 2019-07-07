#include "ShapenetImportModule.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"

#include "fbxsdk.h"
#include "FbxImporter.h"
#include "Editor/UnrealEd/Classes/Factories/FbxAssetImportData.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"

#include "Runtime/Core/Public/Misc/OutputDevice.h"
#include "Runtime/Core/Public/Misc/FeedbackContext.h"
#include "Editor/UnrealEd/Public/FeedbackContextEditor.h"

#include "Developer/AssetTools/Public/AssetToolsModule.h"
#include "Editor/UnrealEd/Public/AutomatedAssetImportData.h"

#include "Editor/UnrealEd/Public/FileHelpers.h"


#include "Shapenet.h"

DEFINE_LOG_CATEGORY(ShapenetImportModule);
IMPLEMENT_GAME_MODULE(FShapenetImportModule, ShapenetImportModule);

#define LOCTEXT_NAMESPACE "ShapenetImportModule"

void FShapenetImportModule::StartupModule()
{
	UE_LOG(ShapenetImportModule, Warning, TEXT("ShapenetImportModule: Log Started"));
	/*
		original test

	FString synset = "04554684";
	//FString synset = "234214123wrong";
	FString hash = "fcc0bdba1a95be2546cde67a6a1ea328";
	bool test = modelAlreadyImported(synset, hash);

	*/

	
	FString synset = "02818832";
	FString hash = "1aa55867200ea789465e08d496c0420f";
	importOBJ(synset, hash);



	//FString synset = "04379243";
	//FString hash = "1a00aa6b75362cc5b324368d54a7416f";
	//importOBJ(synset, hash);

	searchShapenet(synset);

}

void FShapenetImportModule::ShutdownModule()
{
	UE_LOG(ShapenetImportModule, Warning, TEXT("ShapenetImportModule: Log Ended"));
}

bool FShapenetImportModule::importOBJ(FString synset, FString hash)
{
	// do not import if model already imported
	if (modelAlreadyImported(synset, hash)) {
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("expImport: Starting Import"));
	TArray<FString> filesToImport;
	FString srcPath = shapenetDir + "/" + synset + "/" + hash + "/models/model_normalized.obj";
	filesToImport.Add(srcPath);

	UAutomatedAssetImportData* importData = NewObject<UAutomatedAssetImportData>();
	importData->bReplaceExisting = true;

	UFbxFactory* factory = NewObject<UFbxFactory>(UFbxFactory::StaticClass(), FName("Factory"), RF_NoFlags);
	importData->Factory = factory;

	UE_LOG(LogTemp, Warning, TEXT("expImport: Created FBX factory"));

	FString RelativePath = FPaths::ProjectContentDir();
	FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath) + "ShapenetObj/" + synset + "/" + hash;
	FString test = "/Game/ShapenetOBJ/" + synset + "/" + hash + "/";

	importData->DestinationPath = *test;
	importData->Filenames = filesToImport;

	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	auto importedAssets = AssetToolsModule.Get().ImportAssetsAutomated(importData);
	
	UE_LOG(LogTemp, Warning, TEXT("expImport: Finished Import"));
	bool bOutPackagesNeededSaving;
	FEditorFileUtils::SaveDirtyPackages(false, true, true, false, true, false, &bOutPackagesNeededSaving);
	if (!bOutPackagesNeededSaving) {
		UE_LOG(LogTemp, Warning, TEXT("expImport: Nothing to save"));
		return false;
	}
	return true;
}



bool FShapenetImportModule::modelAlreadyImported(FString synset, FString hash)
{
	FString path = "/Game/ShapenetObj/" + synset + "/" + hash + "/model_normalized.model_normalized";
	
	UStaticMesh* mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *path));

	if (mesh != nullptr) {
		UE_LOG(ShapenetImportModule, Warning, TEXT("modelAlreadyImported: Model Already Exists"));
		return true;
	}
	return false;
}



FShapenetImportModule::SearchResult FShapenetImportModule::searchShapenet(FString query)
{
	SearchResult result;
	result.synset = "213123";
	result.numModels = 4;

	FString jsonString;
	FString path = shapenetDir + "/taxonomy.json";
	FFileHelper::LoadFileToString(jsonString, *path);

	UE_LOG(LogTemp, Warning, TEXT("SearchResult: json is %s"), *jsonString);

	FTaxonomy taxonomy;

	return result;
}


#undef LOCTEXT_NAMESPACE