#include "window_surface.h"

#include <cassert>
#include <algorithm>

WindowSurface::WindowSurface(HWND hWnd, const std::wstring& window_name, int client_width, int client_height) :
    m_hwnd(hWnd),
    m_name(window_name),
    m_title(window_name),
    m_client_width(client_width),
    m_client_height(client_height),
    m_previous_mouse_x(0),
    m_previous_mouse_y(0),
    m_is_fullscreen(false),
    m_is_minimized(false),
    m_is_maximized(false),
    m_in_client_rect(false),
    m_has_keyboard_focus(false)
{
    m_dpi_scaling = GetDpiForWindow(hWnd) / 96.0f;
}

WindowSurface::~WindowSurface() {
    DestroyWindow(m_hwnd);
}

HWND WindowSurface::GetWindowHandle() const {
    return m_hwnd;
}

float WindowSurface::GetDPIScaling() const {
    return m_dpi_scaling;
}

void WindowSurface::Show() {
    ShowWindow(m_hwnd, SW_SHOW);
}

void WindowSurface::Hide() {
    ShowWindow(m_hwnd, SW_HIDE);
}

void WindowSurface::OnUpdate(UpdateEventArgs& e) {
    m_timer.Tick();

    e.DeltaTime = m_timer.GetDeltaSeconds();
    e.TotalTime = m_timer.GetTotalSeconds();

    Update(e);
}

void WindowSurface::OnClose(WindowCloseEventArgs& e) {
    Close(e);
}

void WindowSurface::OnResize(ResizeEventArgs& e) {
    m_client_width = e.Width;
    m_client_height = e.Height;

    if ((m_is_minimized || m_is_maximized) && e.State == WindowState::Restored) {
        m_is_maximized = false;
        m_is_minimized = false;
        OnRestored(e);
    }
    if (!m_is_minimized && e.State == WindowState::Minimized) {
        m_is_minimized = true;
        m_is_maximized = false;
        OnMinimized(e);
    }
    if (!m_is_maximized && e.State == WindowState::Maximized) {
        m_is_maximized = true;
        m_is_minimized = false;
        OnMaximized(e);
    }

    Resize(e);
}

void WindowSurface::OnMinimized(ResizeEventArgs& e) {
    Minimized(e);
}

void WindowSurface::OnMaximized(ResizeEventArgs& e) {
    Maximized(e);
}

void WindowSurface::OnRestored(ResizeEventArgs& e) {
    Restored(e);
}

void WindowSurface::OnDPIScaleChanged(DPIScaleEventArgs& e) {
    m_dpi_scaling = e.DPIScale;
    DPIScaleChanged(e);
}

void WindowSurface::OnKeyPressed(KeyEventArgs& e) {
    KeyPressed(e);
}

void WindowSurface::OnKeyReleased(KeyEventArgs& e) {
    KeyReleased(e);
}

void WindowSurface::OnKeyboardFocus(EventArgs& e) {
    m_has_keyboard_focus = true;
    KeyboardFocus(e);
}

void WindowSurface::OnKeyboardBlur(EventArgs& e) {
    m_has_keyboard_focus = false;
    KeyboardBlur(e);
}

void WindowSurface::OnMouseMoved(MouseMotionEventArgs& e) {
    if (!m_in_client_rect) {
        m_previous_mouse_x = e.X;
        m_previous_mouse_y = e.Y;
        m_in_client_rect = true;

        OnMouseEnter(e);
    }

    e.RelX = e.X - m_previous_mouse_x;
    e.RelY = e.Y - m_previous_mouse_y;

    m_previous_mouse_x = e.X;
    m_previous_mouse_y = e.Y;

    MouseMoved(e);
}

void WindowSurface::OnMouseButtonPressed(MouseButtonEventArgs& e) {
    MouseButtonPressed(e);
}

void WindowSurface::OnMouseButtonReleased(MouseButtonEventArgs& e) {
    MouseButtonReleased(e);
}

void WindowSurface::OnMouseWheel(MouseWheelEventArgs& e) {
    MouseWheel(e);
}

void WindowSurface::OnMouseEnter(MouseMotionEventArgs& e) {
    TRACKMOUSEEVENT trackMouseEvent = {};
    trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
    trackMouseEvent.hwndTrack = m_hwnd;
    trackMouseEvent.dwFlags = TME_LEAVE;
    TrackMouseEvent(&trackMouseEvent);

    m_in_client_rect = true;
    MouseEnter(e);
}

void WindowSurface::OnMouseLeave(EventArgs& e) {
    m_in_client_rect = false;
    MouseLeave(e);
}

void WindowSurface::OnMouseFocus(EventArgs& e) {
    MouseFocus(e);
}

void WindowSurface::OnMouseBlur(EventArgs& e) {
    MouseBlur(e);
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
