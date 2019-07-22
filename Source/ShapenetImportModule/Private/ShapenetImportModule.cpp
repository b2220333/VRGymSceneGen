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

#include "Runtime/UMG/Public/Components/Widget.h"
#include "Runtime/UMG/Public/Components/EditableTextBox.h"

#include "Runtime/Core/Public/Internationalization/Regex.h"

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
		ShapenetImportModuleCommands::Get().importShapenetButton,
		FExecuteAction::CreateRaw(this, &FShapenetImportModule::onImportButtonClicked),
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

	
	/*
	FString synset = "02818832";
	FString hash = "1aa55867200ea789465e08d496c0420f";
	importFromSynsetAndHash(synset, hash);
	*/



	//FString synset = "04379243";
	//FString hash = "1a00aa6b75362cc5b324368d54a7416f";
	//importOBJ(synset, hash);

	//searchShapenet(synset);
	//importSynset(synset);



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
	factory->ImportUI->StaticMeshImportData->ImportUniformScale = 100.0f;
	factory->ImportUI->StaticMeshImportData->ImportRotation = FRotator(0.0f, 0.0f, 90.0f);
	factory->ImportUI->StaticMeshImportData->bCombineMeshes = true;
	factory->ImportUI->StaticMeshImportData->bGenerateLightmapUVs = true;
	factory->ImportUI->StaticMeshImportData->bAutoGenerateCollision = true;
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
	

	if (!parsed) {
		UE_LOG(LogTemp, Warning, TEXT("Parse failed "));
	}


	SearchResult result;

	FString patternString = "(^|,)" + query + "(,|$)";
	FRegexPattern pattern = FRegexPattern(patternString);

	for (int32 i = 0; i < synsets.Num(); i++) {
		FRegexMatcher matcher = FRegexMatcher(pattern, synsets[i].name);
		if (matcher.FindNext()) {
			UE_LOG(LogTemp, Warning, TEXT("Found match: %s, %s"), *synsets[i].name, *synsets[i].synsetId);
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

bool FShapenetImportModule::importSynset(FString synset, int32 numToImport)
{
	
	if (synsetExists(synset)) {
		UE_LOG(LogTemp, Warning, TEXT("importSynset: importing %s"), *synset);

		IFileManager& FileManager = IFileManager::Get();
		FString path = shapenetDir + "/" + synset + "/*.*";
		TArray<FString> Hashes;
		FileManager.FindFiles(Hashes, *path, false, true);

		UE_LOG(LogTemp, Warning, TEXT("importSynset: here1"));
		for (int32 i = Hashes.Num() - 1; i > 0; i--) {
			int32 j = FMath::RandRange(0, i - 1);
			FString temp = Hashes[i];
			Hashes[i] = Hashes[j];
			Hashes[j] = temp;
		}
		UE_LOG(LogTemp, Warning, TEXT("importSynset: here2"));

		if (numToImport == -1) {
			numToImport = Hashes.Num();
		}

		bool successfullyImported = true;
		UE_LOG(LogTemp, Warning, TEXT("importSynset: Found %d hashes"), Hashes.Num());
		for (int32 i = 0; i < numToImport; i++) {
			UE_LOG(LogTemp, Warning, TEXT("importSynset: importing %s"), *Hashes[i]);
			successfullyImported = successfullyImported && importFromSynsetAndHash(synset, Hashes[i]);
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

void FShapenetImportModule::onImportButtonClicked()
{
	
	FString path = FPaths::ProjectDir() + "External/import.json";
	UE_LOG(LogTemp, Warning, TEXT("onImportButtonClicked: Importing from %s"), *path);

	FString jsonString;
	FFileHelper::LoadFileToString(jsonString, *path);

	importFromJson(jsonString);
	

	//importFromDir("D:/data/ShapeNetCore.v2/02843684/7a2642b37028b462d39f9e7f9f528702", "/Game/Test");

	//importSynset("02818832");


	// testing text input

	//FObjectInitializer init;
	//UEditableTextBox test = UEditableTextBox(init);

	/*
	FText DialogText = FText::Format(
		LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
		FText::FromString(TEXT("FPLUGIN_NAMEModule::PluginButtonClicked()")),
		FText::FromString(TEXT("PLUGIN_NAME.cpp"))
	);
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
	*/
}

void FShapenetImportModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(ShapenetImportModuleCommands::Get().importShapenetButton);
}

void FShapenetImportModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(ShapenetImportModuleCommands::Get().importShapenetButton);
}


bool FShapenetImportModule::importFromJson(FString json)
{
	FImportJson parsedImportJson;
	bool parsed = FJsonObjectConverter::JsonObjectStringToUStruct<FImportJson>(json, &parsedImportJson, 0, 0);


	UE_LOG(LogTemp, Warning, TEXT("importFromJson: finding search terms"));
	// find synsets from search terms
	for (int32 i = 0; i < parsedImportJson.searchTerms.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("Searching for %s"), *parsedImportJson.searchTerms[i].query);
		SearchResult result = searchShapenet(parsedImportJson.searchTerms[i].query);
		UE_LOG(LogTemp, Warning, TEXT("Found %d synsets"), result.synsets.Num());
		if (result.synsets.Num() > 0) {
			if (parsedImportJson.searchTerms[i].numModelsToImport != -1) {
				int32 totalMatches = 0;
				for (int32 j = 0; j < result.numModels.Num(); j++) {
					totalMatches += result.numModels[j];
				}
				int32 modelsImported = 0;
				for (int32 j = 0; j < result.numModels.Num() - 1; j++) {
					int32 numModelsToImport = (result.numModels[j] * parsedImportJson.searchTerms[i].numModelsToImport) / totalMatches;
					importSynset(result.synsets[j], numModelsToImport);
					modelsImported += numModelsToImport;
				}

				while (modelsImported < parsedImportJson.searchTerms[i].numModelsToImport) {
					importSynset(result.synsets[result.synsets.Num() - 1], 1);
					modelsImported++;
				}
			}
			else {
				for (int32 j = 0; j < result.numModels.Num(); j++) {
					importSynset(result.synsets[j], -1);
				}
			}
		}
	

	}


	UE_LOG(LogTemp, Warning, TEXT("importFromJson: Importing from synsets"));
	// fully import synsets
	for (int32 i = 0; i < parsedImportJson.synsets.Num(); i++) {
		if (parsedImportJson.synsets[i].numModelsToImport != -1) {
			importSynset(parsedImportJson.synsets[i].synset, parsedImportJson.synsets[i].numModelsToImport);
		}
		else {
			importSynset(parsedImportJson.synsets[i].synset, -1);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("importFromJson: Importing from shapenet objects"));
	// import shapenet objects
	for (int32 i = 0; i < parsedImportJson.shapenetObjects.Num(); i++) {
		FShapenetObject* shapenetOBJ = &parsedImportJson.shapenetObjects[i];
		importFromSynsetAndHash(shapenetOBJ->synset, shapenetOBJ->hash);
	}

	UE_LOG(LogTemp, Warning, TEXT("importFromJson: Importing from models manually"));
	// manually import from path
	for (int32 i = 0; i < parsedImportJson.modelsManual.Num(); i++) {
		FModelManual* modelManual = &parsedImportJson.modelsManual[i];
		if (FPaths::FileExists(modelManual->srcPath)) {
			importFromFile(modelManual->srcPath, modelManual->dstPath);
		} else if (FPaths::DirectoryExists(modelManual->srcPath)) {
			importFromDir(modelManual->srcPath, modelManual->dstPath);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("importFromJson: Could not find %s"), *modelManual->srcPath);
		}
		
	}
	

	return false;
}

bool FShapenetImportModule::importFromDir(FString srcPath, FString dstPath)
{
	IFileManager& FileManager = IFileManager::Get();
	TArray<FString> modelFiles;
	FString fileName = "*.obj";
	FileManager.FindFilesRecursive(modelFiles, *srcPath, *fileName, true, false, false);


	for (int32 i = 0; i < modelFiles.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("importFromDir:Importing %s"), *modelFiles[i]);
		importFromFile(srcPath + "/" + modelFiles[i], dstPath + "/" +  modelFiles[i]);
	}

	modelFiles.Empty();

	fileName = "*.fbx";
	FileManager.FindFilesRecursive(modelFiles, *srcPath, *fileName, true, false, false);


	for (int32 i = 0; i < modelFiles.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("importFromDir:Importing %s"), *modelFiles[i]);
		importFromFile(srcPath + "/" + modelFiles[i], dstPath + "/" + modelFiles[i]);
	}




	return false;
}

#undef LOCTEXT_NAMESPACE