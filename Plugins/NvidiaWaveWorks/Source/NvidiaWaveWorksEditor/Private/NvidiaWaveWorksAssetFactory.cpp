#include "NvidiaWaveWorksAssetFactory.h"

#include "NvidiaWaveWorksAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NvidiaWaveWorksAssetFactory)

#define LOCTEXT_NAMESPACE "NvidiaWaveWorksAssetFactory"

UNvidiaWaveWorksAssetFactory::UNvidiaWaveWorksAssetFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UNvidiaWaveWorksAsset::StaticClass();
}

UObject* UNvidiaWaveWorksAssetFactory::FactoryCreateNew(
	UClass* Class,
	UObject* InParent,
	FName Name,
	EObjectFlags Flags,
	UObject* Context,
	FFeedbackContext* Warn)
{
	return NewObject<UNvidiaWaveWorksAsset>(
		InParent,
		Class,
		Name,
		Flags | RF_Transactional);
}

FText UNvidiaWaveWorksAssetFactory::GetToolTip() const
{
	return LOCTEXT(
		"WaveWorksAssetTooltip",
		"Creates an NVIDIA WaveWorks simulation asset");
}

#undef LOCTEXT_NAMESPACE
