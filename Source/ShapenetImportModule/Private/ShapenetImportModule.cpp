#include "ShapenetImportModule.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"

#include "FbxImporter.h"
#include "Editor/UnrealEd/Classes/Factories/FbxAssetImportData.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"

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
	//importOBJ(synset, hash);
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

	UE_LOG(LogTemp, Warning, TEXT("importOBJ: Starting Import"));

	FString path = shapenetDir + "/" + synset + "/" + hash + "/models/model_normalized.obj";
	FString destPath = "/Game/ShapenetOBJ/" + synset + "/" + hash + "/" + "model_normalized";
	//UnFbx::FFbxImporter* FbxImporter = UnFbx::FFbxImporter::GetInstance();
	//UFbxFactory* factory = NewObject<UFbxFactory>(UFbxFactory::StaticClass(), FName("Factory"), RF_NoFlags);

	UFbxFactory* factory = NewObject<UFbxFactory>(UFbxFactory::StaticClass(), FName("Factory"), RF_NoFlags);
	
	//factory->ImportUI->StaticMeshImportData->bConvertScene = true;
	factory->ImportUI->StaticMeshImportData->ImportUniformScale = 999999999;
	factory->ImportUI->AnimSequenceImportData->ImportUniformScale = 999999999;
	factory->ImportUI->SkeletalMeshImportData->ImportUniformScale = 999999999;
	factory->ImportUI->TextureImportData->ImportUniformScale = 999999999;
	//factory->ImportUI->StaticMeshImportData->bCombineMeshes = true;

	FString test = FString::SanitizeFloat(factory->ImportUI->StaticMeshImportData->ImportUniformScale);
	UE_LOG(LogTemp, Warning, TEXT("uniform scale: %s"), *test);

	//factory->ImportUI->StaticMeshImportData->ImportRotation = FRotator(90.0f, 0.0f, 0.0f);

	bool canceled = false;
	UPackage* Package = CreatePackage(NULL, *destPath); //Create package if not exist
	Package->FullyLoad();
	UStaticMesh* mesh = Cast<UStaticMesh>(factory->ImportObject(factory->ResolveSupportedClass(), Package, FName("model_normalized"), RF_Public | RF_Standalone, path, nullptr, canceled));

	factory->ImportUI->StaticMeshImportData->ImportUniformScale = 999999999;
	factory->ImportUI->AnimSequenceImportData->ImportUniformScale = 999999999;
	factory->ImportUI->SkeletalMeshImportData->ImportUniformScale = 999999999;
	factory->ImportUI->TextureImportData->ImportUniformScale = 999999999;


	test = FString::SanitizeFloat(factory->ImportUI->StaticMeshImportData->ImportUniformScale);
	UE_LOG(LogTemp, Warning, TEXT("uniform scale: %s"), *test);

	if (canceled == true)
	{
		// import failed 
		UE_LOG(LogTemp, Warning, TEXT("importOBJ: Import canceled."));
		return false;
	}
	
	FString fileName = "model_normalized.uasset";
	FAssetRegistryModule::AssetCreated(mesh);

	bool bSaved = UPackage::SavePackage(Package, mesh, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *fileName, GError, nullptr, true, true);
	if (bSaved == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("importOBJ: Could not save package"));
		return false;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("importOBJ: Import complete"));
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

#undef LOCTEXT_NAMESPACE