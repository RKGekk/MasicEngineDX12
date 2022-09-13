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
#include "tools/game_timer.h"
#include "render_window_config.h"
#include "engine/engine.h"

class WindowSurface;

class Application {
public:
    static bool Create(HINSTANCE hInst, const ApplicationOptions& opt);
    static void Destroy();
    static Application& Get();

    std::shared_ptr<WindowSurface> CreateRenderWindow(const RenderWindowConfig& cfg);
    static std::shared_ptr<WindowSurface> GetWindowByName(const std::wstring& window_name);
    static std::shared_ptr<WindowSurface> GetWindowByHWND(HWND hwnd);
    static void DestroyWindowByHWND(HWND hwnd);

    bool Run(std::shared_ptr<Engine> pEngine);
    void Quit(int exit_code = 0);
    void Stop();
    
    const ApplicationOptions& GetApplicationOptions();
    GameTimer& GetTimer();

    void CloseWindow(IEventDataPtr pEventData);
    void Paint(IEventDataPtr pEventData);


protected:
    Application(HINSTANCE hInst);
    bool Initialize(const ApplicationOptions& opt);
    virtual ~Application();

    virtual void VRegisterEvents();
    virtual void RegisterAllDelegates();

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