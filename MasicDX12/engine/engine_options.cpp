#include "engine_options.h"

#include "../tools/string_utility.h"

#include <algorithm>

#include <DirectXMath.h>
#include <pugixml/pugixml.hpp>

using namespace std::literals;

EngineOptions::EngineOptions() {}

EngineOptions::EngineOptions(const std::string& xmlFilePath) : EngineOptions() {
	Init(xmlFilePath);
}

EngineOptions::~EngineOptions() {}

bool EngineOptions::Init(const std::string& xml_file_name) {
	pugi::xml_document xml_doc;
	pugi::xml_parse_result parse_res = xml_doc.load_file(xml_file_name.c_str());
	if (!parse_res) { return false;	}

	pugi::xml_node root_node = xml_doc.child("PlayerOptions");
	if (!root_node) { return false; }

	pugi::xml_node graphics_node = root_node.child("Graphics");
	if (graphics_node) {
		m_debug_ui = ntobool(graphics_node.child("DebugUI"), m_debug_ui);
		m_full_screen_max = ntobool(graphics_node.child("FullScreenMax"), m_full_screen_max);
		m_full_screen = ntobool(graphics_node.child("FullScreen"), m_full_screen);
		m_run_full_speed = ntobool(graphics_node.child("RunFullSpeed"), m_run_full_speed);
		m_screen_height = ntofloat(graphics_node.child("Height"), m_screen_height);
		m_screen_width = ntofloat(graphics_node.child("Width"), m_screen_width);

		pugi::xml_node renderer_node = graphics_node.child("Renderer");
		if (renderer_node) {
			std::string sRenderer(renderer_node.first_child().value());
			if (sRenderer == "Direct3D 11"s) m_Renderer = Renderer::Renderer_D3D11;
			else if (sRenderer == "Direct3D 12"s) m_Renderer = Renderer::Renderer_D3D12;
			else if (sRenderer == "Direct3D 9"s) m_Renderer = Renderer::Renderer_D3D9;
			else if (sRenderer == "OpenGL"s) m_Renderer = Renderer::Renderer_OpenGL;
			else if (sRenderer == "Vulkan"s) m_Renderer = Renderer::Renderer_Vulkan;
		}
	}

	pugi::xml_node camera_node = root_node.child("Camera");
	if (camera_node) {
		m_screen_far = ntofloat(camera_node.child("ScreenFar"), m_screen_far);
		m_screen_near = ntofloat(camera_node.child("ScreenNear"), m_screen_near);
		m_fov = ntofloat(camera_node.child("Fov"), m_fov);
		m_game_cam_offset_x = ntofloat(camera_node.child("GameCameraOffsetX"), m_game_cam_offset_x);
		m_game_cam_offset_y = ntofloat(camera_node.child("GameCameraOffsetY"), m_game_cam_offset_y);
		m_game_cam_offset_z = ntofloat(camera_node.child("GameCameraOffsetZ"), m_game_cam_offset_z);
		m_game_cam_rotate_x = ntofloat(camera_node.child("GameCameraRotateX"), m_game_cam_rotate_x);
	}

	pugi::xml_node fog_node = root_node.child("Fog");
	if (fog_node) {
		m_fog_start = ntofloat(camera_node.child("FogStart"), m_fog_start);
		m_fog_range = ntofloat(camera_node.child("FogRange"), m_fog_range);
		m_fog_range = ntofloat(camera_node.child("FogColor"), m_fog_range);
		m_fog_color = colorfromattr4f(fog_node.child("FogColor"));
	}

	pugi::xml_node shadow_node = root_node.child("Shadow");
	if (shadow_node) {
		m_shadow_color = colorfromattr4f(shadow_node.child("ShadowColor"));
	}

	pugi::xml_node audio_node = root_node.child("Sound");
	if (audio_node) {
		m_music_volume = attrtofloat(audio_node.attribute("musicVolume"), m_music_volume);
		m_sound_effects_volume = attrtofloat(audio_node.attribute("sfxVolume"), m_sound_effects_volume);
	}
	
	return true;
}