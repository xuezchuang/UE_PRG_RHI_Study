#include "AssetDefinition_NvidiaHBAOPlusAsset.h"

#include "NvidiaHBAOPlusAsset.h"
#include "NvidiaHBAOPlusAssetEditorToolkit.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AssetDefinition_NvidiaHBAOPlusAsset)

#define LOCTEXT_NAMESPACE "AssetDefinition_NvidiaHBAOPlusAsset"

FText UAssetDefinition_NvidiaHBAOPlusAsset::GetAssetDisplayName() const
{
	return LOCTEXT("DisplayName", "NVIDIA HBAO+");
}

FLinearColor UAssetDefinition_NvidiaHBAOPlusAsset::GetAssetColor() const
{
	return FLinearColor(0.35f, 0.72f, 0.18f);
}

TSoftClassPtr<UObject>
UAssetDefinition_NvidiaHBAOPlusAsset::GetAssetClass() const
{
	return UNvidiaHBAOPlusAsset::StaticClass();
}

TConstArrayView<FAssetCategoryPath>
UAssetDefinition_NvidiaHBAOPlusAsset::GetAssetCategories() const
{
	static const TArray<FAssetCategoryPath> Categories = {
		FAssetCategoryPath(LOCTEXT("RenderingCategory", "Rendering"))};
	return Categories;
}

EAssetCommandResult UAssetDefinition_NvidiaHBAOPlusAsset::OpenAssets(
	const FAssetOpenArgs& OpenArgs) const
{
	for (UNvidiaHBAOPlusAsset* Asset :
		 OpenArgs.LoadObjects<UNvidiaHBAOPlusAsset>())
	{
		TSharedRef<FNvidiaHBAOPlusAssetEditorToolkit> Editor =
			MakeShared<FNvidiaHBAOPlusAssetEditorToolkit>();
		Editor->InitHBAOPlusEditor(
			OpenArgs.GetToolkitMode(),
			OpenArgs.ToolkitHost,
			Asset);
	}

	return EAssetCommandResult::Handled;
}

#undef LOCTEXT_NAMESPACE
