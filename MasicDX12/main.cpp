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

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"DirectXTK12.lib")
#pragma comment(lib,"DXGI.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"D3DCompiler.lib")
#if defined _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <dxgidebug.h>
#pragma comment(lib,"assimp-vc142-mtd.lib")
#else
#pragma comment(lib,"assimp-vc142-mt.lib")
#endif
#pragma comment(lib, "winmm.lib")

#include <dxgi1_6.h>
#include <Windows.h>
#include <Shlwapi.h>
#include <dxgidebug.h>

#include "application_options.h"
#include "application.h"
#include "engine/render_window_config.h"
#include "engine/render_window.h"
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
	int retCode = 0;

	WCHAR path[MAX_PATH];
	HMODULE hModule = GetModuleHandleW(NULL);
	if (GetModuleFileNameW(hModule, path, MAX_PATH) > 0) {
		PathRemoveFileSpecW(path);
		SetCurrentDirectoryW(path);
	}

	ApplicationOptions opt("EngineOptions.xml"s);
	Application::Create(hModule, opt);

	Engine* pEngine = Engine::GetEngine();
	bool can_run = pEngine->Initialize(
		RenderWindowConfig{ ApplicationOptions("EngineOptions.xml"s) }
			.set_hInstance(hInstance)
			.set_window_title("CG2_2")
			.set_window_class("MyTestWindowsClass")
	);
	if (can_run) {
		pEngine->Run();
	}

	atexit(&ReportLiveObjects);

	return retCode;
}