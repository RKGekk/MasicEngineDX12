#pragma once

#include <string>

#include <DirectXMath.h>

#include "renderer_enum.h"

struct EngineOptions {
	Renderer m_Renderer = Renderer::Renderer_D3D12;
	bool m_run_full_speed = false;
	bool m_full_screen = false;
	bool m_full_screen_max = true;
	int m_screen_width = 800;
	int m_screen_height = 600;
	bool m_debug_ui = true;

	float m_screen_near = 0.1f;
	float m_screen_far = 1000.0f;
	float m_fov = 90.0f;
	float m_aspect_ratio = 1.0f;
	float m_game_cam_offset_x = 0.0f;
	float m_game_cam_offset_y = 0.0f;
	float m_game_cam_offset_z = -1.0f;
	float m_game_cam_rotate_x = 0.0f;

	float m_fog_start = 10.0f;
	float m_fog_range = 100.0f;
	DirectX::XMFLOAT4 m_fog_color = { 0.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT4 m_shadow_color = { 1.0f, 1.0f, 1.0f, 1.0f };

	float m_sound_effects_volume = 1.0f;
	float m_music_volume = 1.0f;

	EngineOptions();
	EngineOptions(const std::string& xmlFilePath);
	~EngineOptions();

	bool Init(const std::string& xmlFilePath);
};
