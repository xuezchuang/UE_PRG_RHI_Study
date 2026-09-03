#include "NvidiaHBAOPlusAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NvidiaHBAOPlusAsset)

UNvidiaHBAOPlusAsset::UNvidiaHBAOPlusAsset()
{
}

#if WITH_EDITOR
void UNvidiaHBAOPlusAsset::NotifySettingsChanged()
{
	SettingsChangedDelegate.Broadcast();
}

void UNvidiaHBAOPlusAsset::PostEditChangeProperty(
	FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	NotifySettingsChanged();
}
#endif
