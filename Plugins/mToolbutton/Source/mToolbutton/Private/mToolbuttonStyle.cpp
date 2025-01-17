// Copyright Epic Games, Inc. All Rights Reserved.

#include "mToolbuttonStyle.h"
#include "mToolbutton.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FmToolbuttonStyle::StyleInstance = nullptr;

void FmToolbuttonStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FmToolbuttonStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FmToolbuttonStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("mToolbuttonStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FmToolbuttonStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("mToolbuttonStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("mToolbutton")->GetBaseDir() / TEXT("Resources"));

	Style->Set("mToolbutton.PluginAction", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));
	return Style;
}

void FmToolbuttonStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FmToolbuttonStyle::Get()
{
	return *StyleInstance;
}
