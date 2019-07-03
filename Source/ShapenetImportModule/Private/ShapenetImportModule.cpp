#include "ShapenetImportModule.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"

DEFINE_LOG_CATEGORY(ShapenetImportModule);
IMPLEMENT_GAME_MODULE(FShapenetImportModule, ShapenetImportModule);

#define LOCTEXT_NAMESPACE "ShapenetImportModule"

void FShapenetImportModule::StartupModule()
{
	UE_LOG(ShapenetImportModule, Warning, TEXT("ShapenetImportModule: Log Started"));
	//FString synset = "04554684";
	FString synset = "234214123";
	FString hash = "fcc0bdba1a95be2546cde67a6a1ea328";
	bool test = modelAlreadyImported(synset, hash);
}

void FShapenetImportModule::ShutdownModule()
{
	UE_LOG(ShapenetImportModule, Warning, TEXT("ShapenetImportModule: Log Ended"));
}

bool FShapenetImportModule::importOBJ(FString synset, FString hash)
{
	if (modelAlreadyImported(synset, hash)) {
		return true;
	}
	


	return false;
}

bool FShapenetImportModule::modelAlreadyImported(FString synset, FString hash)
{
	FString path = "/Game/ShapenetObj/" + synset + "/" + hash + "/model_normalized.model_normalized";
	UE_LOG(ShapenetImportModule, Warning, TEXT("here1"));
	
	
	
	//static ConstructorHelpers::FObjectFinder<UStaticMesh>BaseMeshAsset(*path);
	
	UStaticMesh* mesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *path));
	
	
	UE_LOG(ShapenetImportModule, Warning, TEXT("here2"));
	if (mesh != nullptr) {
		UE_LOG(ShapenetImportModule, Warning, TEXT("ShapenetImportModule: Model Already Exists"));
		return true;
	}
	UE_LOG(ShapenetImportModule, Warning, TEXT("ShapenetImportModule: Model Does Not Already Exist"));
	return false;
}

#undef LOCTEXT_NAMESPACE