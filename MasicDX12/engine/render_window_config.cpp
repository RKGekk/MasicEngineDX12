#include "render_window_config.h"

#include "../tools/string_utility.h"

RenderWindowConfig::RenderWindowConfig(const EngineOptions& opt) {
	width = opt.m_screen_width;
	height = opt.m_screen_height;
	is_windowed_mode = !opt.m_full_screen;
	options = opt;
}

RenderWindowConfig& RenderWindowConfig::set_hInstance(HINSTANCE hInstance) {
	this->hInstance = hInstance;
	return *this;
}

RenderWindowConfig& RenderWindowConfig::set_window_title(std::string title) {
	this->window_title_w = s2w(title);
	this->window_title = std::move(title);
	return *this;
}

RenderWindowConfig& RenderWindowConfig::set_window_class(std::string class_name) {
	this->window_class_w = s2w(class_name);
	this->window_class = std::move(class_name);
	return *this;
}

RenderWindowConfig& RenderWindowConfig::set_width(int width) {
	this->width = width;
	return *this;
}

RenderWindowConfig& RenderWindowConfig::set_height(int height) {
	this->height = height;
	return *this;
}

RenderWindowConfig& RenderWindowConfig::set_window_mode(bool mode) {
	this->is_windowed_mode = mode;
	return *this;
}