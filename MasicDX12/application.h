#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include <atomic>
#include <cstdint>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <type_traits>

#include "application_options.h"
#include "events/event_manager.h"
#include "graphics/adapter_reader.h"
#include "graphics/device.h"
#include "tools/game_timer.h"
#include "render_window_config.h"

class WindowSurface;
class EngineImpl;

class Application {
public:
    static void Create(HINSTANCE hInst, const ApplicationOptions& opt);
    static void Destroy();
    static Application& Get();

    std::shared_ptr<WindowSurface> CreateRenderWindow(const RenderWindowConfig& cfg);
    std::shared_ptr<WindowSurface> GetWindowByName(const std::wstring& window_name);

    int Run(std::shared_ptr<EngineImpl> pEngine);
    void Quit(int exit_code = 0);
    void Stop();
    
    GameTimer& GetTimer();

    virtual LRESULT OnWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
    Application(HINSTANCE hInst);
    bool Initialize(const ApplicationOptions& opt);
    virtual ~Application();

    virtual void VRegisterEvents();

private:
    Application(const Application& copy) = delete;
    Application& operator=(const Application& other) = delete;

    HINSTANCE m_hInstance;
    GameTimer m_timer;

    std::unique_ptr<EventManager> m_event_manager;

    std::atomic_bool m_is_running;
    std::atomic_bool m_request_quit;

    ApplicationOptions m_options;
};