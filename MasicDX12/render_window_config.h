#pragma once

#include <Windows.h>

#include <string>

#include "application_options.h"

class RenderWindowConfig {
public:
	RenderWindowConfig() = default;
	RenderWindowConfig(const ApplicationOptions& opt);

	RenderWindowConfig& set_hInstance(HINSTANCE hInstance);
	RenderWindowConfig& set_window_title(std::string title);
	RenderWindowConfig& set_window_class(std::string class_name);
	RenderWindowConfig& set_width(int width);
	RenderWindowConfig& set_height(int height);
	RenderWindowConfig& set_window_mode(bool mode);

	HINSTANCE hInstance;

	std::string window_title;
	std::wstring window_title_w;

	std::string window_class;
	std::wstring window_class_w;

	int width;
	int height;

	bool is_windowed_mode;

	ApplicationOptions options;
};