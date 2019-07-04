#include "ShapenetImportModule.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"

#include "FbxImporter.h"
#include "Editor/UnrealEd/Classes/Factories/FbxAssetImportData.h"

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

	UE_LOG(LogTemp, Warning, TEXT("StartImport"));

	FString path = shapenetDir + "/" + synset + "/" + hash + "/models/model_normalized.obj";
	FString destPath = "/Game/ShapenetOBJ/" + synset + "/" + hash;
	//UnFbx::FFbxImporter* FbxImporter = UnFbx::FFbxImporter::GetInstance();
	//UFbxFactory* factory = NewObject<UFbxFactory>(UFbxFactory::StaticClass(), FName("Factory"), RF_NoFlags);

	UFbxFactory* factory = NewObject<UFbxFactory>(UFbxFactory::StaticClass(), FName("Factory"), RF_NoFlags);
	factory->ImportUI->StaticMeshImportData->ImportUniformScale = 300;
	factory->ImportUI->StaticMeshImportData->bCombineMeshes = true;
	
	//factory->ImportUI->StaticMeshImportData->ImportRotation = FRotator(90.0f, 0.0f, 0.0f);

	bool canceled = false;
	UPackage* Package = CreatePackage(NULL, *destPath); //Create package if not exist
	factory->ImportObject(factory->ResolveSupportedClass(), Package, FName("model_normalized"), RF_Public | RF_Standalone, path, nullptr, canceled);
	
	if (canceled == true)
	{
		// import failed 
		UE_LOG(LogTemp, Warning, TEXT("Import canceled."));
		return false;
	}
	/*
	bool bSaved = UPackage::SavePackage(Package, ImportedMesh, EObjectFlags::RF_Public, *PackageFileName, GError, nullptr, true, true);
	if (bSaved == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not save package"));
		return false;
	}
	*/
	UE_LOG(LogTemp, Warning, TEXT("Saved package"));
	return true;
}

bool FShapenetImportModule::modelAlreadyImported(FString synset, FString hash)
{
	FString path = "/Game/ShapenetObj/" + synset + "/" + hash + "/model_normalized.model_normalized";
	
	UStaticMesh* mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *path));

	if (mesh != nullptr) {
		UE_LOG(ShapenetImportModule, Warning, TEXT("ShapenetImportModule: Model Already Exists"));
		return true;
	}
	return false;
}

#undef LOCTEXT_NAMESPACE