#include "AssetDefinition_NvidiaWaveWorksAsset.h"

#include "NvidiaWaveWorksAsset.h"
#include "NvidiaWaveWorksAssetEditorToolkit.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AssetDefinition_NvidiaWaveWorksAsset)

#define LOCTEXT_NAMESPACE "AssetDefinition_NvidiaWaveWorksAsset"

FText UAssetDefinition_NvidiaWaveWorksAsset::GetAssetDisplayName() const
{
	return LOCTEXT("DisplayName", "NVIDIA WaveWorks");
}

FLinearColor UAssetDefinition_NvidiaWaveWorksAsset::GetAssetColor() const
{
	return FLinearColor(0.0f, 0.45f, 0.78f);
}

TSoftClassPtr<UObject> UAssetDefinition_NvidiaWaveWorksAsset::GetAssetClass() const
{
	return UNvidiaWaveWorksAsset::StaticClass();
}

TConstArrayView<FAssetCategoryPath>
UAssetDefinition_NvidiaWaveWorksAsset::GetAssetCategories() const
{
	static const TArray<FAssetCategoryPath> Categories = {
		FAssetCategoryPath(LOCTEXT("RenderingCategory", "Rendering"))};
	return Categories;
}

EAssetCommandResult UAssetDefinition_NvidiaWaveWorksAsset::OpenAssets(
	const FAssetOpenArgs& OpenArgs) const
{
	for (UNvidiaWaveWorksAsset* Asset :
		 OpenArgs.LoadObjects<UNvidiaWaveWorksAsset>())
	{
		TSharedRef<FNvidiaWaveWorksAssetEditorToolkit> Editor =
			MakeShared<FNvidiaWaveWorksAssetEditorToolkit>();
		Editor->InitWaveWorksEditor(
			OpenArgs.GetToolkitMode(),
			OpenArgs.ToolkitHost,
			Asset);
	}

	return EAssetCommandResult::Handled;
}

#undef LOCTEXT_NAMESPACE
