#include "NvidiaHBAOPlusAssetFactory.h"

#include "NvidiaHBAOPlusAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NvidiaHBAOPlusAssetFactory)

#define LOCTEXT_NAMESPACE "NvidiaHBAOPlusAssetFactory"

UNvidiaHBAOPlusAssetFactory::UNvidiaHBAOPlusAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UNvidiaHBAOPlusAsset::StaticClass();
}

UObject* UNvidiaHBAOPlusAssetFactory::FactoryCreateNew(
	UClass* Class,
	UObject* InParent,
	FName Name,
	EObjectFlags Flags,
	UObject* Context,
	FFeedbackContext* Warn)
{
	return NewObject<UNvidiaHBAOPlusAsset>(
		InParent,
		Class,
		Name,
		Flags | RF_Transactional);
}

FText UNvidiaHBAOPlusAssetFactory::GetToolTip() const
{
	return LOCTEXT(
		"HBAOPlusAssetTooltip",
		"Creates an NVIDIA HBAO+ render-settings asset");
}

#undef LOCTEXT_NAMESPACE
