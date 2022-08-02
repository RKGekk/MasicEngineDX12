#include "application.h"

#include <comdef.h>

#include <algorithm>
#include <cassert>
#include <unordered_map>

#include "engine/engine.h"
#include "window_surface.h"
#include "events/evt_data_os_message.h"


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

LRESULT Application::OnWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    std::shared_ptr<EvtData_OS_Message> pEvent(new EvtData_OS_Message({ hWnd, msg, wParam, lParam }));
    IEventManager::Get()->VTriggerEvent(pEvent);

    return WndProc(hWnd, msg, wParam, lParam);
}

Application::Application(HINSTANCE hInst) : m_hInstance(hInst), m_is_running(false), m_request_quit(false) {}

void Application::VRegisterEvents() {
    REGISTER_EVENT(EvtData_OS_Message);
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
    pWindow->Initialize(*this, cfg);

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


int Application::Run(std::shared_ptr<EngineImpl> pGame) {
    assert(!m_is_running);

    m_is_running = true;

    if (!pGame->Initialize()) return 1;
    if (!pGame->LoadContent()) return 2;
    pGame->ShowWindow();

    MSG msg = { 0 };
    while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) && msg.message != WM_QUIT) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);

        if (m_request_quit) {
            PostQuitMessage(0);
            m_request_quit = false;
        }
    }

    pGame->UnloadContent();

    m_is_running = false;

    return static_cast<int>(msg.wParam);
}

void Application::Quit(int exitCode) {
    PostQuitMessage(exitCode);
}

void Application::Stop() {
    m_request_quit = true;
}

void Application::GetTimer() {
    return m_timer;
}

static MouseButton DecodeMouseButton(UINT messageID) {
    MouseButton mouseButton = MouseButton::None;
    switch (messageID) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        {
            mouseButton = MouseButton::Left;
        }
        break;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        {
            mouseButton = MouseButton::Right;
        }
        break;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        {
            mouseButton = MouseButton::Middle;
        }
        break;
    }

    return mouseButton;
}

static ButtonState DecodeButtonState(UINT messageID) {
    ButtonState buttonState = ButtonState::Pressed;

    switch (messageID) {
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
            buttonState = ButtonState::Released;
            break;
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
            buttonState = ButtonState::Pressed;
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
    std::shared_ptr<WindowSurface> pWindow;
    {
        auto iter = gs_windows.find(hwnd);
        if (iter != gs_windows.end()) {
            pWindow = iter->second.lock();
        }
    }

    if (pWindow) {
        switch (message) {
            case WM_DPICHANGED:
            {
                float dpi_scaling = HIWORD(wParam) / 96.0f;
                DPIScaleEventArgs dpi_scale_event_args(dpi_scaling);
                pWindow->OnDPIScaleChanged(dpi_scale_event_args);
            }
            break;
            case WM_PAINT:
            {
                UpdateEventArgs update_event_args(0.0, 0.0);
                pWindow->OnUpdate(update_event_args);
            }
            break;
            case WM_SYSKEYDOWN:
            case WM_KEYDOWN:
            {
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
            case WM_SYSKEYUP:
            case WM_KEYUP:
            {
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
            case WM_SYSCHAR:
                break;
            case WM_KILLFOCUS:
            {
                EventArgs event_args;
                pWindow->OnKeyboardBlur(event_args);
            }
            break;
            case WM_SETFOCUS:
            {
                EventArgs event_args;
                pWindow->OnKeyboardFocus(event_args);
            }
            break;
            case WM_MOUSEMOVE:
            {
                bool lButton = (wParam & MK_LBUTTON) != 0;
                bool rButton = (wParam & MK_RBUTTON) != 0;
                bool mButton = (wParam & MK_MBUTTON) != 0;
                bool shift = (wParam & MK_SHIFT) != 0;
                bool control = (wParam & MK_CONTROL) != 0;

                int x = ((int)(short)LOWORD(lParam));
                int y = ((int)(short)HIWORD(lParam));

                MouseMotionEventArgs mouse_motion_event_args(lButton, mButton, rButton, control, shift, x, y);
                pWindow->OnMouseMoved(mouse_motion_event_args);
            }
            break;
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            {
                bool lButton = (wParam & MK_LBUTTON) != 0;
                bool rButton = (wParam & MK_RBUTTON) != 0;
                bool mButton = (wParam & MK_MBUTTON) != 0;
                bool shift = (wParam & MK_SHIFT) != 0;
                bool control = (wParam & MK_CONTROL) != 0;

                int x = ((int)(short)LOWORD(lParam));
                int y = ((int)(short)HIWORD(lParam));

                SetCapture(hwnd);

                MouseButtonEventArgs mouse_button_event_args(DecodeMouseButton(message), ButtonState::Pressed, lButton, mButton, rButton, control, shift, x, y);
                pWindow->OnMouseButtonPressed(mouse_button_event_args);
            }
            break;
            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
            {
                bool lButton = (wParam & MK_LBUTTON) != 0;
                bool rButton = (wParam & MK_RBUTTON) != 0;
                bool mButton = (wParam & MK_MBUTTON) != 0;
                bool shift = (wParam & MK_SHIFT) != 0;
                bool control = (wParam & MK_CONTROL) != 0;

                int x = ((int)(short)LOWORD(lParam));
                int y = ((int)(short)HIWORD(lParam));

                ReleaseCapture();

                MouseButtonEventArgs mouse_button_event_args(DecodeMouseButton(message), ButtonState::Released, lButton, mButton, rButton, control, shift, x, y);
                pWindow->OnMouseButtonReleased(mouse_button_event_args);
            }
            break;
            case WM_MOUSEWHEEL:
            {
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
            case WM_CAPTURECHANGED:
            {
                EventArgs mouse_blur_event_args;
                pWindow->OnMouseBlur(mouse_blur_event_args);
            }
            break;
            case WM_MOUSEACTIVATE:
            {
                EventArgs mouse_focus_event_args;
                pWindow->OnMouseFocus(mouse_focus_event_args);
            }
            break;
            case WM_MOUSELEAVE:
            {
                EventArgs mouse_leave_event_args;
                pWindow->OnMouseLeave(mouse_leave_event_args);
            }
            break;
            case WM_SIZE:
            {
                WindowState window_state = DecodeWindowState(wParam);

                int width = ((int)(short)LOWORD(lParam));
                int height = ((int)(short)HIWORD(lParam));

                ResizeEventArgs resize_event_args(width, height, window_state);
                pWindow->OnResize(resize_event_args);
            }
            break;
            case WM_CLOSE:
            {
                WindowCloseEventArgs window_close_event_args;
                pWindow->OnClose(window_close_event_args);

                if (window_close_event_args.ConfirmClose) {
                    pWindow->Hide();
                }
            }
            break;
            case WM_DESTROY:
            {
                std::lock_guard<std::mutex> lock(gs_window_handles_mutex);
                WindowMap::iterator         iter = gs_windows.find(hwnd);
                if (iter != gs_windows.end()) {
                    gs_windows.erase(iter);
                }
            }
            break;
            default:
                return ::DefWindowProcW(hwnd, message, wParam, lParam);
        }
    }
    else {
        switch (message) {
            case WM_CREATE:
                break;
            default:
                return ::DefWindowProcW(hwnd, message, wParam, lParam);
        }
    }

    return 0;
}