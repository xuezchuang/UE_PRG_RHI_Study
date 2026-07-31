#include "INvidiaHBAOPlusModule.h"

#include "HAL/FileManager.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformProcess.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "NvidiaHBAOPlusAsset.h"
#include "NvidiaHBAOPlusRenderer.h"
#include "RendererInterface.h"

DEFINE_LOG_CATEGORY_STATIC(LogNvidiaHBAOPlus, Log, All);

class FNvidiaHBAOPlusModule final : public INvidiaHBAOPlusModule
{
public:
	virtual void StartupModule() override
	{
#if PLATFORM_WINDOWS && WITH_NVIDIA_HBAOPLUS
		const TSharedPtr<IPlugin> Plugin =
			IPluginManager::Get().FindPlugin(TEXT("NvidiaHBAOPlus"));
		if (!Plugin.IsValid())
		{
			UE_LOG(
				LogNvidiaHBAOPlus,
				Error,
				TEXT("Could not resolve the NvidiaHBAOPlus plugin directory"));
			return;
		}

		const FString DllName =
			TEXT("GFSDK_SSAO_D3D12.win64.dll");
		const TArray<FString> DllCandidates = {
			FPaths::Combine(
				Plugin->GetBaseDir(),
				TEXT("Binaries/ThirdParty/HBAOPlus/Win64"),
				DllName),
			FPaths::Combine(
				Plugin->GetBaseDir(),
				TEXT("ThirdParty/HBAOPlus/Bin/Win64"),
				DllName),
			FPaths::Combine(FPlatformProcess::BaseDir(), DllName)};

		FString LoadedDllPath;
		for (const FString& DllPath : DllCandidates)
		{
			if (IFileManager::Get().FileExists(*DllPath))
			{
				HBAOPlusDllHandle =
					FPlatformProcess::GetDllHandle(*DllPath);
				if (HBAOPlusDllHandle != nullptr)
				{
					LoadedDllPath = DllPath;
					break;
				}
			}
		}

		if (HBAOPlusDllHandle == nullptr)
		{
			UE_LOG(
				LogNvidiaHBAOPlus,
				Error,
				TEXT("Failed to load the HBAO+ SDK DLL"));
			return;
		}

		UE_LOG(
			LogNvidiaHBAOPlus,
			Log,
			TEXT("Loaded HBAO+ SDK DLL: %s"),
			*LoadedDllPath);
#endif

		Renderer = MakeUnique<FNvidiaHBAOPlusRenderer>();
		// PostConfigInit runs before UObject class default objects are available.
		// FNvidiaHBAOPlusRenderSettings mirrors the asset defaults and is safe
		// to construct during this early module phase.
		Renderer->UpdateSettings(FNvidiaHBAOPlusRenderSettings());

		IRendererModule& RendererModule =
			FModuleManager::LoadModuleChecked<IRendererModule>(
				TEXT("Renderer"));
		PostOpaqueDelegateHandle =
			RendererModule.RegisterPostOpaqueRenderDelegate(
				FPostOpaqueRenderDelegate::CreateRaw(
					Renderer.Get(),
					&FNvidiaHBAOPlusRenderer::AddPostOpaquePass));

#if !UE_BUILD_SHIPPING
		ResetContextCommand =
			IConsoleManager::Get().RegisterConsoleCommand(
				TEXT("NvidiaHBAOPlus.ResetContext"),
				TEXT("Waits for the GPU and recreates HBAO+ contexts."),
				FConsoleCommandDelegate::CreateRaw(
					this,
					&FNvidiaHBAOPlusModule::RequestContextReset),
				ECVF_Default);

		StatusCommand =
			IConsoleManager::Get().RegisterConsoleCommand(
				TEXT("NvidiaHBAOPlus.Status"),
				TEXT("Prints the current NVIDIA HBAO+ renderer status."),
				FConsoleCommandDelegate::CreateRaw(
					this,
					&FNvidiaHBAOPlusModule::LogStatus),
				ECVF_Default);
#endif

		UE_LOG(
			LogNvidiaHBAOPlus,
			Log,
			TEXT("NVIDIA HBAO+ runtime module loaded"));
	}

	virtual void ShutdownModule() override
	{
#if !UE_BUILD_SHIPPING
		if (ResetContextCommand != nullptr)
		{
			IConsoleManager::Get().UnregisterConsoleObject(
				ResetContextCommand,
				false);
			ResetContextCommand = nullptr;
		}
		if (StatusCommand != nullptr)
		{
			IConsoleManager::Get().UnregisterConsoleObject(
				StatusCommand,
				false);
			StatusCommand = nullptr;
		}
#endif

		if (PostOpaqueDelegateHandle.IsValid() &&
			FModuleManager::Get().IsModuleLoaded(TEXT("Renderer")))
		{
			IRendererModule& RendererModule =
				FModuleManager::GetModuleChecked<IRendererModule>(
					TEXT("Renderer"));
			RendererModule.RemovePostOpaqueRenderDelegate(
				PostOpaqueDelegateHandle);
			PostOpaqueDelegateHandle.Reset();
		}

		if (Renderer)
		{
			Renderer->Shutdown();
			Renderer.Reset();
		}

#if PLATFORM_WINDOWS && WITH_NVIDIA_HBAOPLUS
		if (HBAOPlusDllHandle != nullptr)
		{
			FPlatformProcess::FreeDllHandle(HBAOPlusDllHandle);
			HBAOPlusDllHandle = nullptr;
		}
#endif

		UE_LOG(
			LogNvidiaHBAOPlus,
			Log,
			TEXT("NVIDIA HBAO+ runtime module unloaded"));
	}

	virtual void ApplyAssetSettings(
		const UNvidiaHBAOPlusAsset& Asset) override
	{
		if (Renderer)
		{
			Renderer->UpdateSettings(
				FNvidiaHBAOPlusRenderSettings::FromAsset(Asset));
		}
	}

	virtual void RequestContextReset() override
	{
		if (Renderer)
		{
			Renderer->RequestContextReset();
		}
	}

	virtual FString GetStatus() const override
	{
		return Renderer
			? Renderer->GetStatus()
			: TEXT("Runtime renderer is unavailable");
	}

private:
#if !UE_BUILD_SHIPPING
	void LogStatus()
	{
		UE_LOG(
			LogNvidiaHBAOPlus,
			Log,
			TEXT("%s"),
			*GetStatus());
	}

	IConsoleObject* ResetContextCommand = nullptr;
	IConsoleObject* StatusCommand = nullptr;
#endif

	TUniquePtr<FNvidiaHBAOPlusRenderer> Renderer;
	FDelegateHandle PostOpaqueDelegateHandle;
	void* HBAOPlusDllHandle = nullptr;
};

IMPLEMENT_MODULE(FNvidiaHBAOPlusModule, NvidiaHBAOPlus)
