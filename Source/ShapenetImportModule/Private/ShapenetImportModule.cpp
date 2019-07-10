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

#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformFile.h"
#include "Runtime/Core/Public/HAL/FileManager.h"

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "Runtime/Core/Public/Templates/SharedPointer.h"
#include "Runtime/Slate/Public/Framework/Commands/Commands.h"
#include "ShapenetImportModuleCommands.h"

static const FName ImportShapenet("ShapenetImportModule");

DEFINE_LOG_CATEGORY(ShapenetImportModule);
IMPLEMENT_GAME_MODULE(FShapenetImportModule, ShapenetImportModule);

#define LOCTEXT_NAMESPACE "ShapenetImportModule"

void FShapenetImportModule::StartupModule()
{
	UE_LOG(ShapenetImportModule, Warning, TEXT("ShapenetImportModule: Log Started"))

	

	// testing menu editing extension

	ShapenetImportModuleStyle::Initialize();
	ShapenetImportModuleStyle::ReloadTextures();

	ShapenetImportModuleCommands::Register();

	ModuleCommands = MakeShareable(new FUICommandList);

	ModuleCommands->MapAction(
		ShapenetImportModuleCommands::Get().importShapenetAllCommand,
		FExecuteAction::CreateRaw(this, &FShapenetImportModule::importShapenetAll),
		FCanExecuteAction());

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, ModuleCommands, FMenuExtensionDelegate::CreateRaw(this, &FShapenetImportModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}

	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, ModuleCommands, FToolBarExtensionDelegate::CreateRaw(this, &FShapenetImportModule::AddToolbarExtension));

		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}



	/*
		original test

	FString synset = "04554684";
	//FString synset = "234214123wrong";
	FString hash = "fcc0bdba1a95be2546cde67a6a1ea328";
	bool test = modelAlreadyImported(synset, hash);

	*/

	
	FString synset = "02818832";
	FString hash = "1aa55867200ea789465e08d496c0420f";
	importFromSynsetAndHash(synset, hash);



	//FString synset = "04379243";
	//FString hash = "1a00aa6b75362cc5b324368d54a7416f";
	//importOBJ(synset, hash);

	searchShapenet(synset);
	importSynset(synset);



}

void FShapenetImportModule::ShutdownModule()
{
	UE_LOG(ShapenetImportModule, Warning, TEXT("ShapenetImportModule: Log Ended"));
}

bool FShapenetImportModule::importFromSynsetAndHash(FString synset, FString hash)
{
	// do not import if model already imported
	if (modelAlreadyImported(synset, hash)) {
		return true;
	}
	FString srcPath = shapenetDir + "/" + synset + "/" + hash + "/models/model_normalized.obj";
	FString RelativePath = FPaths::ProjectContentDir();
	FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath) + "ShapenetObj/" + synset + "/" + hash;
	FString dstPath = "/Game/ShapenetOBJ/" + synset + "/" + hash + "/";
	return importFromFile(srcPath, dstPath);
}

bool FShapenetImportModule::importFromFile(FString srcPath, FString dstPath)
{
	UE_LOG(LogTemp, Warning, TEXT("importFromFile: Starting Import"));
	TArray<FString> filesToImport;
	filesToImport.Add(srcPath);

	UAutomatedAssetImportData* importData = NewObject<UAutomatedAssetImportData>();
	importData->bReplaceExisting = true;

	UFbxFactory* factory = NewObject<UFbxFactory>(UFbxFactory::StaticClass(), FName("Factory"), RF_NoFlags);
	importData->Factory = factory;

	UE_LOG(LogTemp, Warning, TEXT("importFromFile: Created FBX factory"));

	importData->DestinationPath = *dstPath;
	importData->Filenames = filesToImport;

	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	auto importedAssets = AssetToolsModule.Get().ImportAssetsAutomated(importData);

	UE_LOG(LogTemp, Warning, TEXT("importFromFile: Finished Import"));
	bool bOutPackagesNeededSaving;
	FEditorFileUtils::SaveDirtyPackages(false, true, true, false, true, false, &bOutPackagesNeededSaving);
	if (!bOutPackagesNeededSaving) {
		UE_LOG(LogTemp, Warning, TEXT("importFromFile: Nothing to save"));
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

	FString jsonString;
	FString path = shapenetDir + "/taxonomy.json";
	FFileHelper::LoadFileToString(jsonString, *path);

	//UE_LOG(LogTemp, Warning, TEXT("SearchResult: json is %s"), *jsonString);

	TArray<FSynsetObj> synsets;

	bool parsed = FJsonObjectConverter::JsonArrayStringToUStruct<FSynsetObj>(jsonString, &synsets, 0, 0);

	if (parsed) {
		FString test = synsets[0].name;
		UE_LOG(LogTemp, Warning, TEXT("First synset name is %s"), *test);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Parse failed "));
	}


	SearchResult result;

	for (int32 i = 0; i < result.synsets.Num(); i++) {
		if (synsets[i].name.Contains(query)) {
			result.synsets.Add(synsets[i].synsetId);
			result.numModels.Add(synsets[i].numInstances);
		}
	}


	return result;
}

bool FShapenetImportModule::synsetExists(FString query)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString path = shapenetDir + "/" + query;
	
	return PlatformFile.DirectoryExists(*path);
}

bool FShapenetImportModule::importSynset(FString synset)
{
	
	if (synsetExists(synset)) {
		UE_LOG(LogTemp, Warning, TEXT("importSynset: importing %s"), *synset);
		
		IFileManager& FileManager = IFileManager::Get();
		FString path = shapenetDir + "/" + synset + "/*.*";
		TArray<FString> Hashes;
		FileManager.FindFiles(Hashes, *path, false, true);

		bool successfullyImported = true;
		UE_LOG(LogTemp, Warning, TEXT("importSynset: Found %d hashes"), Hashes.Num());
		for (int32 i = 0; i < Hashes.Num(); i++) {
			UE_LOG(LogTemp, Warning, TEXT("importSynset: importing %s"), *Hashes[i]);
			//successfullyImported = successfullyImported && importOBJ(synset, Hashes[i]);
		}
		
		return successfullyImported;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("importSynset: synset does not exist in shapenet"), *synset);
		return false;
	}
	return false;

}

void FShapenetImportModule::setShapenetDir(FString path)
{
	shapenetDir = path;
}

FString FShapenetImportModule::getShapenetDir()
{
	return shapenetDir;
}

void FShapenetImportModule::importShapenetAll()
{
	UE_LOG(LogTemp, Warning, TEXT("importShapenetAll: wow this command works"));
}

void FShapenetImportModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(ShapenetImportModuleCommands::Get().importShapenetAllCommand);
}

void FShapenetImportModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(ShapenetImportModuleCommands::Get().importShapenetAllCommand);
}

#undef LOCTEXT_NAMESPACE