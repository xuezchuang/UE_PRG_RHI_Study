#include "Modules/ModuleManager.h"

#include "NvidiaWaveWorksEditorCommands.h"

class FNvidiaWaveWorksEditorModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		FNvidiaWaveWorksEditorCommands::Register();
	}

	virtual void ShutdownModule() override
	{
		FNvidiaWaveWorksEditorCommands::Unregister();
	}
};

IMPLEMENT_MODULE(FNvidiaWaveWorksEditorModule, NvidiaWaveWorksEditor)
