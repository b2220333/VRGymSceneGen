#include "ShapenetImportModuleStyle.h"
#include "ShapenetImportModule.h"
#include "SlateGameResources.h"
#include "IPluginManager.h"

TSharedPtr< FSlateStyleSet > ShapenetImportModuleStyle::StyleInstance = NULL;

void ShapenetImportModuleStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		if (StyleInstance.IsValid())
			FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void ShapenetImportModuleStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName ShapenetImportModuleStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ShapenetImportModuleStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > ShapenetImportModuleStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("ShapenetImportModuleStyle"));
	
	if (StyleInstance.IsValid()) {
		Style->SetContentRoot(IPluginManager::Get().FindPlugin("ShapenetImportModule")->GetBaseDir() / TEXT("Resources"));

		Style->Set("ShapenetImportModule.importShapenetAllCommand", new IMAGE_BRUSH(TEXT("ButtonIcon_40x"), Icon40x40));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("ShapenetImportModuleStyle: Style is invalid"));
	} 
	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void ShapenetImportModuleStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& ShapenetImportModuleStyle::Get()
{
	return *StyleInstance;
}