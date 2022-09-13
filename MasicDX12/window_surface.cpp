#include "window_surface.h"

#include "application.h"
#include "engine/engine.h"
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
#include "events/evt_data_os_message.h"
#include "events/evt_data_resize_window.h"
#include "events/evt_data_restore_window.h"
#include "events/evt_data_window_close.h"
#include "events/evt_data_paint.h"

#include <cassert>
#include <algorithm>

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

WindowSurface::WindowSurface() : m_dpi_scaling(1.0f), m_hInstance(), m_hwnd(), m_name(), m_title(), m_client_width(1u), m_client_height(1u), m_previous_mouse_x(0), m_previous_mouse_y(0), m_is_fullscreen(false), m_is_minimized(false), m_is_maximized(false) {}

WindowSurface::~WindowSurface() {
    DestroyWindow(m_hwnd);
    m_hwnd = NULL;
    UnregisterClass(m_name.c_str(), m_hInstance);
    m_hInstance = NULL;
}

bool WindowSurface::Initialize(const RenderWindowConfig& cfg) {
    m_hInstance = cfg.hInstance;

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

    m_hwnd = CreateWindowExW(
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
    assert(m_hwnd && "Failed to create window");
    if (!m_hwnd) {
        MessageBoxA(NULL, "Could not create the render window.", "Error", MB_OK | MB_ICONERROR);
        return false;
    }
    m_dpi_scaling = GetDpiForWindow(m_hwnd) / 96.0f;
    VRegisterEvents();

    return true;
}

bool WindowSurface::ProcessMessages() {
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    BOOL res = PeekMessage(&msg, m_hwnd, 0, 0, PM_REMOVE);
    if(res){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT) {
            res = false;
        }
    }

    return res;
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
    std::shared_ptr<EvtData_Window_Close> pEvent = std::make_shared<EvtData_Window_Close>(m_hwnd, confirm_close);
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
    REGISTER_EVENT(EvtData_Paint);
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
    REGISTER_EVENT(EvtData_OS_Message);
}

void WindowSurface::OnWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    std::shared_ptr<EvtData_OS_Message> pEvent(new EvtData_OS_Message({ hWnd, msg, wParam, lParam }));
    IEventManager::Get()->VTriggerEvent(pEvent);
}

void WindowSurface::RegisterWindowClass() {
    WNDCLASSEX wc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
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

HWND WindowSurface::GetHWND() const {
    return m_hwnd;
}

HINSTANCE WindowSurface::GetInstance() const {
    return m_hInstance;
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

static MouseButtonSide DecodeMouseButton(UINT messageID) {
    MouseButtonSide mouseButton = MouseButtonSide::None;
    switch (messageID) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        {
            mouseButton = MouseButtonSide::Left;
        }
        break;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        {
            mouseButton = MouseButtonSide::Right;
        }
        break;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        {
            mouseButton = MouseButtonSide::Middle;
        }
        break;
    }

    return mouseButton;
}

static MKState DecodeButtonState(UINT messageID) {
    MKState buttonState = MKState::Pressed;

    switch (messageID) {
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
            buttonState = MKState::Released;
            break;
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
            buttonState = MKState::Pressed;
            break;
    }

    return buttonState;
}

static WindowState DecodeWindowState(WPARAM wParam) {
    WindowState windowState = WindowState::Restored;

    switch (wParam) {
        case SIZE_RESTORED:
            windowState = WindowState::Restored;
            break;
        case SIZE_MINIMIZED:
            windowState = WindowState::Minimized;
            break;
        case SIZE_MAXIMIZED:
            windowState = WindowState::Maximized;
            break;
        default:
            break;
    }

    return windowState;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    std::shared_ptr<WindowSurface> pWindow = Application::GetWindowByHWND(hwnd);
    std::shared_ptr<Engine> pEngine = Engine::GetEngine();

    if (!pWindow) {
        switch (message) {
            case WM_CREATE:
                break;
            default:
                return ::DefWindowProcW(hwnd, message, wParam, lParam);
        }
        return 0;
    }

    pWindow->OnWndProc(hwnd, message, wParam, lParam);
    if (pEngine) {
        LRESULT res = pEngine->WindowProc(hwnd, message, wParam, lParam);
        if (res) return res;
    }

    switch (message) {
        case WM_DPICHANGED : {
            float dpi_scaling = HIWORD(wParam) / 96.0f;
            pWindow->OnDPIScaleChanged(dpi_scaling);
        }
        break;
        case WM_PAINT : {
            std::shared_ptr<EvtData_Paint> pEvent(new EvtData_Paint());
            IEventManager::Get()->VTriggerEvent(pEvent);
        }
        break;
        case WM_SYSKEYDOWN :
        case WM_KEYDOWN : {
            MSG charMsg;
            unsigned int c = 0;
            if (PeekMessage(&charMsg, hwnd, 0, 0, PM_NOREMOVE) && charMsg.message == WM_CHAR) {
                c = static_cast<unsigned int>(charMsg.wParam);
            }

            bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

            WindowKey key = (WindowKey)wParam;
            KeyEventArgs key_event_args(key, c, KeyState::Pressed, control, shift, alt);
            pWindow->OnKeyPressed(key_event_args);
        }
        break;
        case WM_SYSKEYUP :
        case WM_KEYUP : {
            bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
            bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

            WindowKey key = (WindowKey)wParam;
            unsigned int c = 0;
            unsigned int scanCode = (lParam & 0x00FF0000) >> 16;

            unsigned char keyboard_state[256];
            GetKeyboardState(keyboard_state);
            wchar_t translated_characters[4];
            if (int result = ToUnicodeEx((UINT)wParam, scanCode, keyboard_state, translated_characters, 4, 0, NULL) > 0) {
                c = translated_characters[0];
            }

            KeyEventArgs key_event_args(key, c, KeyState::Released, control, shift, alt);
            pWindow->OnKeyReleased(key_event_args);
        }
        break;
        case WM_SYSCHAR :
            break;
        case WM_KILLFOCUS :
            break;
        case WM_SETFOCUS :
            break;
        case WM_MOUSEMOVE : {
            bool lButton = (wParam & MK_LBUTTON) != 0;
            bool rButton = (wParam & MK_RBUTTON) != 0;
            bool mButton = (wParam & MK_MBUTTON) != 0;
            bool shift = (wParam & MK_SHIFT) != 0;
            bool control = (wParam & MK_CONTROL) != 0;

            int x = ((int)(short)LOWORD(lParam));
            int y = ((int)(short)HIWORD(lParam));

            MouseMotionEventArgs mouse_motion_event_args(lButton, mButton, rButton, control, shift, x, y, 0, 0);
            pWindow->OnMouseMoved(mouse_motion_event_args);
        }
        break;
        case WM_LBUTTONDOWN :
        case WM_RBUTTONDOWN :
        case WM_MBUTTONDOWN : {
            bool lButton = (wParam & MK_LBUTTON) != 0;
            bool rButton = (wParam & MK_RBUTTON) != 0;
            bool mButton = (wParam & MK_MBUTTON) != 0;
            bool shift = (wParam & MK_SHIFT) != 0;
            bool control = (wParam & MK_CONTROL) != 0;

            int x = ((int)(short)LOWORD(lParam));
            int y = ((int)(short)HIWORD(lParam));

            SetCapture(hwnd);

            MBEventArgs mouse_button_event_args(DecodeMouseButton(message), MKState::Pressed, lButton, mButton, rButton, control, shift, x, y);
            pWindow->OnMouseButtonPressed(mouse_button_event_args);
        }
        break;
        case WM_LBUTTONUP :
        case WM_RBUTTONUP :
        case WM_MBUTTONUP : {
            bool lButton = (wParam & MK_LBUTTON) != 0;
            bool rButton = (wParam & MK_RBUTTON) != 0;
            bool mButton = (wParam & MK_MBUTTON) != 0;
            bool shift = (wParam & MK_SHIFT) != 0;
            bool control = (wParam & MK_CONTROL) != 0;

            int x = ((int)(short)LOWORD(lParam));
            int y = ((int)(short)HIWORD(lParam));

            ReleaseCapture();

            MBEventArgs mouse_button_event_args(DecodeMouseButton(message), MKState::Released, lButton, mButton, rButton, control, shift, x, y);
            pWindow->OnMouseButtonReleased(mouse_button_event_args);
        }
        break;
        case WM_MOUSEWHEEL : {
            float zDelta = ((int)(short)HIWORD(wParam)) / (float)WHEEL_DELTA;
            short key_states = (short)LOWORD(wParam);

            bool lButton = (key_states & MK_LBUTTON) != 0;
            bool rButton = (key_states & MK_RBUTTON) != 0;
            bool mButton = (key_states & MK_MBUTTON) != 0;
            bool shift = (key_states & MK_SHIFT) != 0;
            bool control = (key_states & MK_CONTROL) != 0;

            int x = ((int)(short)LOWORD(lParam));
            int y = ((int)(short)HIWORD(lParam));

            POINT screen_to_client_point;
            screen_to_client_point.x = x;
            screen_to_client_point.y = y;
            ::ScreenToClient(hwnd, &screen_to_client_point);

            MouseWheelEventArgs mouse_wheel_event_args(zDelta, lButton, mButton, rButton, control, shift, (int)screen_to_client_point.x, (int)screen_to_client_point.y);
            pWindow->OnMouseWheel(mouse_wheel_event_args);
        }
        break;
        case WM_CAPTURECHANGED :
            break;
        case WM_MOUSEACTIVATE :
            break;
        case WM_MOUSELEAVE :
            break;
        case WM_SIZE: {
            WindowState window_state = DecodeWindowState(wParam);

            int width = ((int)(short)LOWORD(lParam));
            int height = ((int)(short)HIWORD(lParam));

            ResizeEventArgs resize_event_args(width, height, window_state);
            pWindow->OnResize(resize_event_args);
        }
        break;
        case WM_CLOSE: {
            pWindow->OnClose(true);
            pWindow->Hide();
        }
        break;
        case WM_DESTROY: {
            Application::Get().DestroyWindowByHWND(hwnd);
        }
        break;
        default:
            return ::DefWindowProcW(hwnd, message, wParam, lParam);
    }

    return 0;
}