#include "application.h"

#include <comdef.h>

#include <algorithm>
#include <cassert>
#include <unordered_map>

#include "events/evt_data_update_tick.h"
#include "window_surface.h"

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

constexpr wchar_t WINDOW_CLASS_NAME[] = L"DX12RenderWindowClass";

using WindowPtr = std::weak_ptr<WindowSurface>;
using WindowMap = std::unordered_map<HWND, WindowPtr>;
using WindowNameMap = std::unordered_map<std::wstring, WindowPtr>;

static Application* gs_pSingeton = nullptr;
static WindowMap gs_windows;
static WindowNameMap gs_window_by_name;
static std::mutex gs_window_handles_mutex;

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

Application::Application(HINSTANCE hInst) : m_hInstance(hInst), m_is_running(false), m_request_quit(false) {}

void Application::VRegisterEvents() {
    REGISTER_EVENT(EvtData_Update_Tick);
}

bool Application::Initialize(const ApplicationOptions& opt) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        _com_error err(hr);
        throw new std::exception(ConvertString(err.ErrorMessage()).c_str());
    }

    m_timer.Start();
    m_options = opt;
    m_event_manager = std::make_unique<EventManager>("GameCodeApp Event Mgr", true);
    if (!m_event_manager) {
        return false;
    }

    return true;
}

void Application::Create(HINSTANCE hInst, const ApplicationOptions& opt) {
    if (!gs_pSingeton) {
        gs_pSingeton = new Application(hInst);
        gs_pSingeton->Initialize(opt);
    }
}

Application& Application::Get() {
    assert(gs_pSingeton);
    return *gs_pSingeton;
}

void Application::Destroy() {
    if (gs_pSingeton) {
        delete gs_pSingeton;
        gs_pSingeton = nullptr;
    }
}

Application::~Application() {
    gs_windows.clear();
    gs_window_by_name.clear();
}

std::shared_ptr<WindowSurface> Application::CreateRenderWindow(const RenderWindowConfig& cfg) {
    std::shared_ptr<WindowSurface> pWindow = std::make_shared<WindowSurface>();
    pWindow->Initialize(cfg);

    gs_windows.insert(WindowMap::value_type(pWindow->GetHWND(), pWindow));
    gs_window_by_name.insert(WindowNameMap::value_type(pWindow->GetWindowTitle(), pWindow));

    return pWindow;
}

std::shared_ptr<WindowSurface> Application::GetWindowByName(const std::wstring& window_name) {
    std::shared_ptr<WindowSurface> window;
    WindowNameMap::iterator iter = gs_window_by_name.find(window_name);
    if (iter != gs_window_by_name.end()) {
        window = iter->second.lock();
    }
    return window;
}

std::shared_ptr<WindowSurface> Application::GetWindowByHWND(HWND hwnd) {
    std::shared_ptr<WindowSurface> pWindow;
    auto iter = gs_windows.find(hwnd);
    if (iter != gs_windows.end()) {
        pWindow = iter->second.lock();
    }
    return pWindow;
}

std::shared_ptr<WindowSurface> Application::DestroyWindowByHWND(HWND hwnd) {
    std::lock_guard<std::mutex> lock(gs_window_handles_mutex);
    WindowMap::iterator iter = gs_windows.find(hwnd);
    if (iter != gs_windows.end()) {
        gs_windows.erase(iter);
    }
}


bool Application::Run(std::shared_ptr<Engine> pGame, const RenderWindowConfig& cfg) {
    assert(!m_is_running);

    m_is_running = true;

    if (!pGame->Initialize(cfg)) return 1;
    pGame->ShowWindow();

    while (pGame->ProcessMessages()) {
        m_timer.Tick();
        if (m_request_quit) {
            PostQuitMessage(0);
            m_request_quit = false;
        }
    }

    m_is_running = false;

    return true;
}

void Application::Quit(int exitCode) {
    PostQuitMessage(exitCode);
}

void Application::Stop() {
    m_request_quit = true;
}

const GameTimer& Application::GetTimer() {
    return m_timer;
}