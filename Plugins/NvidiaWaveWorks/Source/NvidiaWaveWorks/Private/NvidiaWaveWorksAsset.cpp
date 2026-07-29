#include "NvidiaWaveWorksAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NvidiaWaveWorksAsset)

UNvidiaWaveWorksAsset::UNvidiaWaveWorksAsset()
{
}

#if WITH_EDITOR
void UNvidiaWaveWorksAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	SettingsChangedDelegate.Broadcast();
}
#endif
