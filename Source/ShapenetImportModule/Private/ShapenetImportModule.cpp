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


#include "json.hpp"
using json = nlohmann::json;

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
	

}

void FShapenetImportModule::ShutdownModule()
{
	UE_LOG(ShapenetImportModule, Warning, TEXT("ShapenetImportModule: Log Ended"));
}

bool FShapenetImportModule::importFromSynsetAndHash(FString synset, FString hash)
{
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

	std::string jsonStr = std::string(TCHAR_TO_UTF8(*jsonString));
	json parsed = json::parse(jsonStr);

	SearchResult result;
	if (parsed.is_array()) {
		FString patternString = "(^|,)" + query + "(,|$)";
		FRegexPattern pattern = FRegexPattern(patternString);

		json::array_t& synsets = parsed.get_ref<json::array_t&>();

		for (auto it = synsets.begin(); it != synsets.end(); it++) {
			if ((*it)["name"].is_string()) {
				FString name = FString((*it)["name"].get<json::string_t>().c_str());
				FRegexMatcher matcher = FRegexMatcher(pattern, name);
				if (matcher.FindNext()) {
					if ((*it)["synsetId"].is_string() && (*it)["numInstances"].is_number_integer()) {
						FString synsetId = FString((*it)["synsetId"].get<json::string_t>().c_str());
						int32 numInstances = (*it)["numInstances"].get<json::number_integer_t>();
						UE_LOG(LogTemp, Warning, TEXT("Found match: %s, %s"), *name, *synsetId);
						result.synsets.Add(synsetId);
						result.numModels.Add(numInstances);
					}
				}
			}
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

		for (int32 i = Hashes.Num() - 1; i > 0; i--) {
			int32 j = FMath::RandRange(0, i - 1);
			FString temp = Hashes[i];
			Hashes[i] = Hashes[j];
			Hashes[j] = temp;
		}

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

	tryImportPartnetQuery("chair", 5);
	/*
	Main 


	FString path = FPaths::ProjectDir() + "External/import.json";
	UE_LOG(LogTemp, Warning, TEXT("onImportButtonClicked: Importing from %s"), *path);

	FString jsonString;
	FFileHelper::LoadFileToString(jsonString, *path);

	importFromJson(jsonString);
	*/


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


bool FShapenetImportModule::importFromJson(FString jsonString)
{	
	std::string jsonStr = std::string(TCHAR_TO_UTF8(*jsonString));
	json parsed = json::parse(jsonStr);

	UE_LOG(LogTemp, Warning, TEXT("importFromJson: finding search terms"));
	// find synsets from search terms

	if (parsed["searchTerms"].is_array()) {
		json::array_t& searchTerms = parsed["searchTerms"].get_ref<json::array_t&>();
		for (auto it = searchTerms.begin(); it != searchTerms.end(); it++) {
			if ((*it)["query"].is_string()) {
				FString query = FString((*it)["query"].get<json::string_t>().c_str());
				UE_LOG(LogTemp, Warning, TEXT("Searching for %s"), *query);
				SearchResult result = searchShapenet(query);
				UE_LOG(LogTemp, Warning, TEXT("Found %d synsets"), result.synsets.Num());
				if (result.synsets.Num() > 0) {
					if ((*it)["numModelsToImport"].is_number_integer()) {
						int32 numImport = (*it)["numModelsToImport"].get<json::number_integer_t>();
						int32 totalMatches = 0;
						for (int32 j = 0; j < result.numModels.Num(); j++) {
							totalMatches += result.numModels[j];
						}
						int32 modelsImported = 0;
						for (int32 j = 0; j < result.numModels.Num() - 1; j++) {
							int32 numModelsToImport = (result.numModels[j] * numImport) / totalMatches;
							importSynset(result.synsets[j], numModelsToImport);
							modelsImported += numModelsToImport;
						}

						while (modelsImported < numImport) {
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
		}
	}


	UE_LOG(LogTemp, Warning, TEXT("importFromJson: Importing from synsets"));
	// fully import synsets
	if (parsed["synsets"].is_array()) {
		json::array_t& synsets = parsed["synsets"].get_ref<json::array_t&>();
		for (auto it = synsets.begin(); it != synsets.end(); it++) {
			if (it->is_string() && it->find("numModelsToImport") != it->end()) {
				std::string synsetStr = it->get<json::string_t>();
				FString synset = FString(synsetStr.c_str());
				if ((*it)["numModelsToImport"].is_number()) {
					int32 numImport = (*it)["numModelsToImport"].get<json::number_integer_t>();
					importSynset(synset, numImport);
				} else {
					importSynset(synset, -1);
				}
			}
			
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("importFromJson: Importing from shapenet objects"));
	// import shapenet objects
	if (parsed["shapenetObjects"].is_array()) {
		json::array_t& shapenetObjects = parsed["shapenetObjects"].get_ref<json::array_t&>();
		for (auto it = shapenetObjects.begin(); it != shapenetObjects.end(); it++) {
			if ((*it)["synset"].is_string() && (*it)["hash"].is_string()) {
				FString synset = FString((*it)["synset"].get<json::string_t>().c_str());
				FString hash = FString((*it)["hash"].get<json::string_t>().c_str());
				importFromSynsetAndHash(synset, hash);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("importFromJson: Importing from models manually"));
	// manually import from path
	if (parsed["modelsManual"].is_array()) {
		json::array_t& manualModels = parsed["modelsManual"].get_ref<json::array_t&>();
		for (auto it = manualModels.begin(); it != manualModels.end(); it++) {
			if ((*it)["srcPath"].is_string() && (*it)["dstPath"].is_string()) {
				FString srcPath = FString((*it)["srcPath"].get<json::string_t>().c_str());
				FString dstPath = FString((*it)["dstPath"].get<json::string_t>().c_str());
				if (FPaths::FileExists(srcPath)) {
					importFromFile(srcPath, dstPath);
				}
				else if (FPaths::DirectoryExists(srcPath)) {
					importFromDir(srcPath, dstPath);
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("importFromJson: Could not find %s"), *srcPath);
				}
			}
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
		FString test = dstPath + "/" + modelFiles[i].Replace(*srcPath, TEXT(""));
		UE_LOG(LogTemp, Warning, TEXT("importFromDir:Importing %s to %s"), *modelFiles[i], *test);
		importFromFile(modelFiles[i], dstPath);
	}

	modelFiles.Empty();

	fileName = "*.fbx";
	FileManager.FindFilesRecursive(modelFiles, *srcPath, *fileName, true, false, false);


	for (int32 i = 0; i < modelFiles.Num(); i++) {
		FString test = dstPath + "/" + modelFiles[i].Replace(*srcPath, TEXT(""));
		UE_LOG(LogTemp, Warning, TEXT("importFromDir:Importing %s to %s"), *modelFiles[i], *test);
		importFromFile(modelFiles[i], dstPath);
	}
	return false;
}


TArray<FString> FShapenetImportModule::searchPartnet(FString query)
{
	FString path = partnetDir + "/partnet_dataset/stats/all_valid_anno_info.txt";
	FString annotationInfo;
	FFileHelper::LoadFileToString(annotationInfo, *path);
	FString delimitter = "\n";

	TArray<FString> annotationIds;

	TArray<FString> lines;
	annotationInfo.ParseIntoArray(lines, *delimitter, true);

	for (int32 i = 0; i < lines.Num(); i++) {
		FString line = lines[i];
		FString delimitter2 = " ";
		TArray<FString> columns;
		line.ParseIntoArray(columns, *delimitter2, true);
		if (columns.Num() == 5 && columns[2] == query.ToLower()) {
			annotationIds.Add(columns[0]);
		}
	}
	return annotationIds;
}

void FShapenetImportModule::importPartnetFromAnnotationID(FString annotationID)
{
	
	json::object_t metaJson= getPartnetMetaJson(annotationID);
	
	if (metaJson["model_cat"].is_string()) {
		FString srcPath = partnetDir + "/" + annotationID + "/objs";
		FString dstPath = "/Game/partnetOBJ/" + FString(metaJson["model_cat"].get<json::string_t>().c_str()) + "/" + annotationID + "/";
		importFromDir(srcPath, dstPath);
	}

	
	
}

void FShapenetImportModule::tryImportPartnetQuery(FString query, int32 numToImport)
{
	TArray<FString> searchResults = searchPartnet(query);

	for (int32 i = searchResults.Num() - 1; i > 0; i--) {
		int32 j = FMath::RandRange(0, i - 1);
		FString temp = searchResults[i];
		searchResults[i] = searchResults[j];
		searchResults[j] = temp;
	}

	if (numToImport == -1) {
		numToImport = searchResults.Num();
	}
	for (int32 i = 0; i < numToImport; i++) {
		importPartnetFromAnnotationID(searchResults[i]);
	}


}

json::object_t FShapenetImportModule::getPartnetMetaJson(FString annotationID)
{
	json::object_t metaJson;
	FString path = partnetDir + +"/" + annotationID + "/meta.json";
	FString metaJsonStr;
	FFileHelper::LoadFileToString(metaJsonStr, *path);
	json parsed = json::parse(std::string(TCHAR_TO_UTF8(*metaJsonStr)));

	if (parsed.is_object()) {
		metaJson = parsed.get<json::object_t>();
	}
	return metaJson;
}


#undef LOCTEXT_NAMESPACE