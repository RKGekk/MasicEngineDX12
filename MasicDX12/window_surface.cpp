#include "window_surface.h"

#include "application.h"
#include "events/evt_data_window_close.h"
#include "events/evt_data_dpi_scale.h"
#include "events/evt_data_key_pressed_event.h"
#include "events/evt_data_key_released_event.h"
#include "events/evt_data_maximize_window.h"
#include "events/evt_data_minimize_window.h"
#include "events/evt_data_mouse_button_pressed.h"
#include "events/evt_data_mouse_button_released.h"
#include "events/evt_data_mouse_motion.h"
#include "events/evt_data_mouse_wheel.h"
#include "events/evt_data_resize_window.h"
#include "events/evt_data_restore_window.h"
#include "events/evt_data_update_tick.h"
#include "events/evt_data_window_close.h"

#include <cassert>
#include <algorithm>

WindowSurface::WindowSurface() : m_hwnd(), m_name(), m_title(), m_client_width(1u), m_client_height(1u), m_previous_mouse_x(0), m_previous_mouse_y(0), m_is_fullscreen(false), m_is_minimized(false), m_is_maximized(false) {}

WindowSurface::~WindowSurface() {
    DestroyWindow(m_hwnd);
    m_hwnd = NULL;
    UnregisterClass(m_name.c_str(), m_hInstance);
    m_hInstance = NULL;
}

bool WindowSurface::Initialize(Application& windowContainer, const RenderWindowConfig& cfg) {
    m_hInstance = cfg.hInstance;
    m_dpi_scaling = GetDpiForWindow(m_hwnd) / 96.0f;

    m_name = cfg.window_class_w;
    m_title = cfg.window_title_w;
    m_client_width = cfg.width;
    m_client_height = cfg.height;
    m_is_fullscreen = cfg.options.FullScreen;

    RegisterWindowClass();

    /*static bool raw_input_initialized = false;
    if (!raw_input_initialized) {
        RAWINPUTDEVICE rid;
        rid.usUsagePage = 0x01;
        rid.usUsage = 0x02;
        rid.dwFlags = 0;
        rid.hwndTarget = 0;
        if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
            DWORD err = GetLastError();
            throw("Failed to register raw input device.");
            return false;
        }
        raw_input_initialized = true;
    }*/

    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);

    RECT window_rect = { 0, 0, static_cast<LONG>(m_client_width), static_cast<LONG>(m_client_height) };
    AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, FALSE);

    uint32_t width = window_rect.right - window_rect.left;
    uint32_t height = window_rect.bottom - window_rect.top;

    int window_x = std::max<int>(0, (screen_width - (int)width) / 2);
    int window_y = std::max<int>(0, (screen_height - (int)height) / 2);

    DWORD style = WS_OVERLAPPEDWINDOW;

    HWND hWnd = CreateWindowExW(
        NULL,
        m_name.c_str(),
        m_name.c_str(),
        style,
        window_x,
        window_y,
        width,
        height,
        NULL,
        NULL,
        cfg.hInstance,
        NULL
    );
    assert(hWnd && "Failed to create window");
    if (!hWnd) {
        MessageBoxA(NULL, "Could not create the render window.", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

HWND WindowSurface::GetWindowHandle() const {
    return m_hwnd;
}

float WindowSurface::GetDPIScaling() const {
    return m_dpi_scaling;
}

void WindowSurface::Show() {
    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);
}

void WindowSurface::Hide() {
    ShowWindow(m_hwnd, SW_HIDE);
}

void WindowSurface::OnClose(bool confirm_close) {
    std::shared_ptr<EvtData_Window_Close> pEvent(new EvtData_Window_Close(confirm_close));
    IEventManager::Get()->VTriggerEvent(pEvent);
}

void WindowSurface::OnResize(ResizeEventArgs& e) {
    m_client_width = e.Width;
    m_client_height = e.Height;

    if ((m_is_minimized || m_is_maximized) && e.State == WindowState::Restored) {
        m_is_maximized = false;
        m_is_minimized = false;
        std::shared_ptr<EvtData_Resize_Window> pEvent(new EvtData_Resize_Window(e));
        IEventManager::Get()->VTriggerEvent(pEvent);
    }
    if (!m_is_minimized && e.State == WindowState::Minimized) {
        m_is_minimized = true;
        m_is_maximized = false;
        std::shared_ptr<EvtData_Resize_Window> pEvent(new EvtData_Resize_Window(e));
        IEventManager::Get()->VTriggerEvent(pEvent);
    }
    if (!m_is_maximized && e.State == WindowState::Maximized) {
        m_is_maximized = true;
        m_is_minimized = false;
        std::shared_ptr<EvtData_Resize_Window> pEvent(new EvtData_Resize_Window(e));
        IEventManager::Get()->VTriggerEvent(pEvent);
    }
}

void WindowSurface::OnMinimized(ResizeEventArgs& e) {
    std::shared_ptr<EvtData_Minimize_Window> pEvent(new EvtData_Minimize_Window(e));
    IEventManager::Get()->VTriggerEvent(pEvent);
}

void WindowSurface::OnMaximized(ResizeEventArgs& e) {
    std::shared_ptr<EvtData_Maximize_Window> pEvent(new EvtData_Maximize_Window(e));
    IEventManager::Get()->VTriggerEvent(pEvent);
}

void WindowSurface::OnRestored(ResizeEventArgs& e) {
    std::shared_ptr<EvtData_Restore_Window> pEvent(new EvtData_Restore_Window(e));
    IEventManager::Get()->VTriggerEvent(pEvent);
}

void WindowSurface::OnDPIScaleChanged(float dpi_scale) {
    std::shared_ptr<EvtData_DPI_Scale> pEvent(new EvtData_DPI_Scale(dpi_scale));
    IEventManager::Get()->VTriggerEvent(pEvent);
}

void WindowSurface::OnKeyPressed(KeyEventArgs& e) {
    std::shared_ptr<EvtData_Key_Pressed_Event> pEvent(new EvtData_Key_Pressed_Event(e));
    IEventManager::Get()->VTriggerEvent(pEvent);
}

void WindowSurface::OnKeyReleased(KeyEventArgs& e) {
    std::shared_ptr<EvtData_Key_Released_Event> pEvent(new EvtData_Key_Released_Event(e));
    IEventManager::Get()->VTriggerEvent(pEvent);
}

void WindowSurface::OnMouseMoved(MouseMotionEventArgs& e) {
    e.RelX = e.X - m_previous_mouse_x;
    e.RelY = e.Y - m_previous_mouse_y;

    m_previous_mouse_x = e.X;
    m_previous_mouse_y = e.Y;

    std::shared_ptr<EvtData_Mouse_Motion> pEvent(new EvtData_Mouse_Motion(e));
    IEventManager::Get()->VTriggerEvent(pEvent);
}

void WindowSurface::OnMouseButtonPressed(MBEventArgs& e) {
    std::shared_ptr<EvtData_Mouse_Button_Pressed> pEvent(new EvtData_Mouse_Button_Pressed(e));
    IEventManager::Get()->VTriggerEvent(pEvent);
}

void WindowSurface::OnMouseButtonReleased(MBEventArgs& e) {
    std::shared_ptr<EvtData_Mouse_Button_Released> pEvent(new EvtData_Mouse_Button_Released(e));
    IEventManager::Get()->VTriggerEvent(pEvent);
}

void WindowSurface::OnMouseWheel(MouseWheelEventArgs& e) {
    std::shared_ptr<EvtData_Mouse_Wheel> pEvent(new EvtData_Mouse_Wheel(e));
    IEventManager::Get()->VTriggerEvent(pEvent);
}

void WindowSurface::VRegisterEvents() {
    REGISTER_EVENT(EvtData_Window_Close);
    REGISTER_EVENT(EvtData_Resize_Window);
    REGISTER_EVENT(EvtData_Minimize_Window);
    REGISTER_EVENT(EvtData_Maximize_Window);
    REGISTER_EVENT(EvtData_Restore_Window);
    REGISTER_EVENT(EvtData_DPI_Scale);
    REGISTER_EVENT(EvtData_Key_Pressed_Event);
    REGISTER_EVENT(EvtData_Key_Released_Event);
    REGISTER_EVENT(EvtData_Mouse_Motion);
    REGISTER_EVENT(EvtData_Mouse_Button_Pressed);
    REGISTER_EVENT(EvtData_Mouse_Button_Released);
    REGISTER_EVENT(EvtData_Mouse_Wheel);
}

LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Application* const pWindow = reinterpret_cast<Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    return pWindow->OnWndProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_NCCREATE:
        {
            const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
            Application* pWindow = reinterpret_cast<Application*>(pCreate->lpCreateParams);
            if (!pWindow) {
                throw("Error: pointer to window container is null during WM_NCCREATE.");
                exit(-1);
            }
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
            SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));

            return pWindow->OnWndProc(hwnd, uMsg, wParam, lParam);
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void WindowSurface::RegisterWindowClass() {
    WNDCLASSEX wc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = HandleMessageSetup;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hInstance;
    wc.hIcon = LoadIcon(m_hInstance, NULL);
    wc.hIconSm = LoadIcon(m_hInstance, NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = m_name.c_str();
    wc.cbSize = sizeof(WNDCLASSEX);

    static ATOM atom = RegisterClassEx(&wc);
    assert(atom > 0);
}

void WindowSurface::SetWindowTitle(const std::wstring& window_title) {
    m_title = window_title;
    SetWindowTextW(m_hwnd, m_title.c_str());
}

const std::wstring& WindowSurface::GetWindowTitle() const {
    return m_title;
}

int WindowSurface::GetClientWidth() const {
    return m_client_width;
}

int WindowSurface::GetClientHeight() const {
    return m_client_height;
}

bool WindowSurface::IsFullscreen() const {
    return m_is_fullscreen;
}

void WindowSurface::SetFullscreen(bool fullscreen) {
    if (m_is_fullscreen != fullscreen) {
        m_is_fullscreen = fullscreen;

        if (m_is_fullscreen) {
            GetWindowRect(m_hwnd, &m_window_rect);

            UINT window_style = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
            SetWindowLongW(m_hwnd, GWL_STYLE, window_style);

            HMONITOR hMonitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFOEX monitor_info = {};
            monitor_info.cbSize = sizeof(MONITORINFOEX);
            GetMonitorInfo(hMonitor, &monitor_info);

            SetWindowPos(
                m_hwnd,
                HWND_TOP,
                monitor_info.rcMonitor.left,
                monitor_info.rcMonitor.top,
                monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
                monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
                SWP_FRAMECHANGED | SWP_NOACTIVATE
            );

            ShowWindow(m_hwnd, SW_MAXIMIZE);
        }
        else {
            SetWindowLong(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

            SetWindowPos(
                m_hwnd,
                HWND_NOTOPMOST,
                m_window_rect.left,
                m_window_rect.top,
                m_window_rect.right - m_window_rect.left,
                m_window_rect.bottom - m_window_rect.top,
                SWP_FRAMECHANGED | SWP_NOACTIVATE
            );

            ShowWindow(m_hwnd, SW_NORMAL);
        }
    }
}

void WindowSurface::ToggleFullscreen() {
    SetFullscreen(!m_is_fullscreen);
}
