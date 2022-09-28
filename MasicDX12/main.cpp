#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

#include <utility>
#include <tuple>
#include <string>

//#pragma comment(lib,"rpcrt4.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"DirectXTK12.lib")
#pragma comment(lib,"DXGI.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"D3DCompiler.lib")
#pragma comment(lib,"DirectXTex.lib")
#if defined _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <dxgidebug.h>
#pragma comment(lib,"assimp-vc142-mtd.lib")
#else
#pragma comment(lib,"assimp-vc142-mt.lib")
#endif
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Shlwapi.lib")

#include <dxgi1_6.h>
#include <Windows.h>
#include <Shlwapi.h>
#include <dxgidebug.h>

#include "application_options.h"
#include "application.h"
#include "render_window_config.h"
#include "window_surface.h"
#include "engine/engine.h"
#include "events/event_manager.h"

using namespace std::literals;

void ReportLiveObjects() {
	IDXGIDebug1* dxgiDebug;
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug));

	HRESULT hr = dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_IGNORE_INTERNAL);
	dxgiDebug->Release();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	int ret_code = 0;

	WCHAR path[MAX_PATH];
	HMODULE hModule = GetModuleHandleW(NULL);
	if (GetModuleFileNameW(hModule, path, MAX_PATH) > 0) {
		PathRemoveFileSpecW(path);
		SetCurrentDirectoryW(path);
	}

	RenderWindowConfig cfg(ApplicationOptions("application_options.xml"s));
	cfg.set_hInstance(hInstance);
	cfg.set_window_title("CG2_2");
	cfg.set_window_class("MyTestWindowsClass");

	bool can_run = Application::Create(hModule, cfg.options);
	std::shared_ptr<Engine> pEngine = Engine::GetEngine();
	if (!can_run || !pEngine) return -1;

	can_run = pEngine->Initialize(cfg);
	if (can_run) {
		Application::Get().Run(pEngine);
	}

	Engine::Destroy();
	pEngine.reset();
	
	Application::Destroy();

	atexit(&ReportLiveObjects);

	return ret_code;
}