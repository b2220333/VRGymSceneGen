// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "SceneGenUnreal/SceneGenUnrealGameModeBase.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeSceneGenUnrealGameModeBase() {}
// Cross Module References
	SCENEGENUNREAL_API UClass* Z_Construct_UClass_ASceneGenUnrealGameModeBase_NoRegister();
	SCENEGENUNREAL_API UClass* Z_Construct_UClass_ASceneGenUnrealGameModeBase();
	ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
	UPackage* Z_Construct_UPackage__Script_SceneGenUnreal();
// End Cross Module References
	void ASceneGenUnrealGameModeBase::StaticRegisterNativesASceneGenUnrealGameModeBase()
	{
	}
	UClass* Z_Construct_UClass_ASceneGenUnrealGameModeBase_NoRegister()
	{
		return ASceneGenUnrealGameModeBase::StaticClass();
	}
	struct Z_Construct_UClass_ASceneGenUnrealGameModeBase_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_ASceneGenUnrealGameModeBase_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AGameModeBase,
		(UObject* (*)())Z_Construct_UPackage__Script_SceneGenUnreal,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ASceneGenUnrealGameModeBase_Statics::Class_MetaDataParams[] = {
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering Utilities|Transformation" },
		{ "IncludePath", "SceneGenUnrealGameModeBase.h" },
		{ "ModuleRelativePath", "SceneGenUnrealGameModeBase.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_ASceneGenUnrealGameModeBase_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ASceneGenUnrealGameModeBase>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_ASceneGenUnrealGameModeBase_Statics::ClassParams = {
		&ASceneGenUnrealGameModeBase::StaticClass,
		DependentSingletons, ARRAY_COUNT(DependentSingletons),
		0x009002A8u,
		nullptr, 0,
		nullptr, 0,
		nullptr,
		&StaticCppClassTypeInfo,
		nullptr, 0,
		METADATA_PARAMS(Z_Construct_UClass_ASceneGenUnrealGameModeBase_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_ASceneGenUnrealGameModeBase_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_ASceneGenUnrealGameModeBase()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_ASceneGenUnrealGameModeBase_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(ASceneGenUnrealGameModeBase, 498474545);
	static FCompiledInDefer Z_CompiledInDefer_UClass_ASceneGenUnrealGameModeBase(Z_Construct_UClass_ASceneGenUnrealGameModeBase, &ASceneGenUnrealGameModeBase::StaticClass, TEXT("/Script/SceneGenUnreal"), TEXT("ASceneGenUnrealGameModeBase"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(ASceneGenUnrealGameModeBase);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
