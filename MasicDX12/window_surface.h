#pragma once

#include "render_window_config.h"
#include "tools/game_timer.h"
#include "events/i_event_manager.h"
#include "events/resize_event_args.h"
#include "events/key_event_args.h"
#include "events/mouse_button_event_args.h"
#include "events/mouse_motion_event_args.h"
#include "events/mouse_wheel_event_args.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <memory>
#include <string>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

class Application;

class WindowSurface {
public:
    HWND GetWindowHandle() const;

    float GetDPIScaling() const;

    const std::wstring& GetWindowName() const;
    void SetWindowTitle(const std::wstring& window_title);
    const std::wstring& GetWindowTitle() const;
    HWND GetHWND() const;
    HINSTANCE GetInstance() const;

    int GetClientWidth() const;
    int GetClientHeight() const;

    bool IsFullscreen() const;
    void SetFullscreen(bool fullscreen);
    void ToggleFullscreen();

    void Show();
    void Hide();

    WindowSurface();

    WindowSurface(const WindowSurface&) = delete;
    WindowSurface& operator=(const WindowSurface& right) = delete;
    WindowSurface(WindowSurface&&) = delete;
    WindowSurface&& operator=(WindowSurface&& right) = delete;

    virtual ~WindowSurface();

    virtual void OnDPIScaleChanged(float dpi_scale);

    virtual void OnClose(bool confirm_close);
    virtual void OnResize(ResizeEventArgs& e);
    virtual void OnMinimized(ResizeEventArgs& e);
    virtual void OnMaximized(ResizeEventArgs& e);
    virtual void OnRestored(ResizeEventArgs& e);

    virtual void OnKeyPressed(KeyEventArgs& e);
    virtual void OnKeyReleased(KeyEventArgs& e);

    virtual void OnMouseMoved(MouseMotionEventArgs& e);
    virtual void OnMouseButtonPressed(MBEventArgs& e);
    virtual void OnMouseButtonReleased(MBEventArgs& e);
    virtual void OnMouseWheel(MouseWheelEventArgs& e);

    virtual bool Initialize(const RenderWindowConfig& cfg);
    virtual void VRegisterEvents();
    virtual void OnWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    virtual bool ProcessMessages();

protected:
    void RegisterWindowClass();

private:
    HWND m_hwnd;
    HINSTANCE m_hInstance;

    std::wstring m_name;
    std::wstring m_title;

    uint32_t m_client_width;
    uint32_t m_client_height;

    int32_t m_previous_mouse_x;
    int32_t m_previous_mouse_y;

    float m_dpi_scaling;

    bool m_is_fullscreen;
    bool m_is_minimized;
    bool m_is_maximized;

    RECT m_window_rect;
};
