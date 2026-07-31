#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class UNvidiaHBAOPlusAsset;

class NVIDIAHBAOPLUS_API INvidiaHBAOPlusModule : public IModuleInterface
{
public:
	static INvidiaHBAOPlusModule& Get()
	{
		return FModuleManager::LoadModuleChecked<INvidiaHBAOPlusModule>(
			TEXT("NvidiaHBAOPlus"));
	}

	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded(TEXT("NvidiaHBAOPlus"));
	}

	virtual void ApplyAssetSettings(const UNvidiaHBAOPlusAsset& Asset) = 0;
	virtual void RequestContextReset() = 0;
	virtual FString GetStatus() const = 0;
};
