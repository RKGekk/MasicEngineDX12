#pragma once

#include <Windows.h>

#include "render_window_config.h"

class RenderWindow {
public:
	RenderWindow() = default;
	RenderWindow(const RenderWindow&) = delete;
	RenderWindow& operator=(const RenderWindow& right) = delete;

	bool Initialize(const RenderWindowConfig& cfg);
	bool ProcessMessages();
	~RenderWindow();

	HWND GetHWND() const;
	HINSTANCE GetInstance() const;
	int GetWidth() const;
	int GetHeight() const;

	const RenderWindowConfig& GetConfig() const;

private:
	void RegisterWindowClass();

	HWND m_handle = NULL;
	RenderWindowConfig m_window_config;
};