#include "Modules/ModuleManager.h"

#include "NvidiaHBAOPlusEditorCommands.h"

class FNvidiaHBAOPlusEditorModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		FNvidiaHBAOPlusEditorCommands::Register();
	}

	virtual void ShutdownModule() override
	{
		FNvidiaHBAOPlusEditorCommands::Unregister();
	}
};

IMPLEMENT_MODULE(FNvidiaHBAOPlusEditorModule, NvidiaHBAOPlusEditor)
