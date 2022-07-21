#pragma once

#include "../events/events.h"
#include "../tools/game_timer.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <memory>
#include <string>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

class WindowSurface {
public:
    HWND GetWindowHandle() const;

    float GetDPIScaling() const;

    const std::wstring& GetWindowName() const;
    void SetWindowTitle(const std::wstring& window_title);
    const std::wstring& GetWindowTitle() const;

    int GetClientWidth() const;
    int GetClientHeight() const;

    bool IsFullscreen() const;
    void SetFullscreen(bool fullscreen);
    void ToggleFullscreen();

    void Show();
    void Hide();

    UpdateEvent Update;
    DPIScaleEvent DPIScaleChanged;
    WindowCloseEvent Close;
    ResizeEvent Resize;
    ResizeEvent Minimized;
    ResizeEvent Maximized;
    ResizeEvent Restored;
    KeyboardEvent KeyPressed;
    KeyboardEvent KeyReleased;
    Event KeyboardFocus;
    Event KeyboardBlur;
    MouseMotionEvent MouseMoved;
    MouseMotionEvent MouseEnter;
    MouseButtonEvent MouseButtonPressed;
    MouseButtonEvent MouseButtonReleased;
    MouseWheelEvent MouseWheel;
    Event MouseLeave;
    Event MouseFocus;
    Event MouseBlur;

protected:
    friend class GameFramework;
    friend LRESULT CALLBACK::WndProc(HWND, UINT, WPARAM, LPARAM);

    WindowSurface(HWND hWnd, const std::wstring& window_name, int client_width, int client_height);

    virtual ~WindowSurface();

    virtual void OnUpdate(UpdateEventArgs& e);
    virtual void OnDPIScaleChanged(DPIScaleEventArgs& e);

    virtual void OnClose(WindowCloseEventArgs& e);
    virtual void OnResize(ResizeEventArgs& e);
    virtual void OnMinimized(ResizeEventArgs& e);
    virtual void OnMaximized(ResizeEventArgs& e);
    virtual void OnRestored(ResizeEventArgs& e);

    virtual void OnKeyPressed(KeyEventArgs& e);
    virtual void OnKeyReleased(KeyEventArgs& e);
    virtual void OnKeyboardFocus(EventArgs& e);
    virtual void OnKeyboardBlur(EventArgs& e);

    virtual void OnMouseMoved(MouseMotionEventArgs& e);
    virtual void OnMouseButtonPressed(MouseButtonEventArgs& e);
    virtual void OnMouseButtonReleased(MouseButtonEventArgs& e);
    virtual void OnMouseWheel(MouseWheelEventArgs& e);

    virtual void OnMouseEnter(MouseMotionEventArgs& e);
    virtual void OnMouseLeave(EventArgs& e);
    virtual void OnMouseFocus(EventArgs& e);
    virtual void OnMouseBlur(EventArgs& e);

private:
    HWND m_hwnd;

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

    bool m_in_client_rect;
    RECT m_window_rect;

    bool m_has_keyboard_focus;

    GameTimer m_timer;
};
