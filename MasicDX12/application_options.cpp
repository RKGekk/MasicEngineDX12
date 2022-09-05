#include "application_options.h"

#include "../tools/string_utility.h"

#include <algorithm>

#include <DirectXMath.h>
#include <pugixml/pugixml.hpp>

using namespace std::literals;

ApplicationOptions::ApplicationOptions() {}

ApplicationOptions::ApplicationOptions(const std::string& xmlFilePath) : ApplicationOptions() {
	Init(xmlFilePath);
}

ApplicationOptions::~ApplicationOptions() {}

bool ApplicationOptions::Init(const std::string& xml_file_name) {
	pugi::xml_document xml_doc;
	pugi::xml_parse_result parse_res = xml_doc.load_file(xml_file_name.c_str());
	if (!parse_res) { return false;	}

	//RootNode = xml_doc.child("PlayerOptions");
	RootNode = xml_doc.root();
	if (!RootNode) { return false; }

	pugi::xml_node graphics_node = RootNode.child("Graphics");
	if (graphics_node) {
		DebugUI = ntobool(graphics_node.child("DebugUI"), DebugUI);
		FullScreenMax = ntobool(graphics_node.child("FullScreenMax"), FullScreenMax);
		FullScreen = ntobool(graphics_node.child("FullScreen"), FullScreen);
		RunFullSpeed = ntobool(graphics_node.child("RunFullSpeed"), RunFullSpeed);
		ScreenHeight = ntoint(graphics_node.child("Height"), ScreenHeight);
		ScreenWidth = ntoint(graphics_node.child("Width"), ScreenWidth);

		pugi::xml_node renderer_node = graphics_node.child("Renderer");
		if (renderer_node) {
			std::string sRenderer(renderer_node.first_child().value());
			if (sRenderer == "Direct3D 11"s) Renderer = Renderer::Renderer_D3D11;
			else if (sRenderer == "Direct3D 12"s) Renderer = Renderer::Renderer_D3D12;
			else if (sRenderer == "Direct3D 9"s) Renderer = Renderer::Renderer_D3D9;
			else if (sRenderer == "OpenGL"s) Renderer = Renderer::Renderer_OpenGL;
			else if (sRenderer == "Vulkan"s) Renderer = Renderer::Renderer_Vulkan;
		}
	}

	pugi::xml_node audio_node = RootNode.child("Sound");
	if (audio_node) {
		MusicVolume = attrtofloat(audio_node.attribute("musicVolume"), MusicVolume);
		SoundEffectsVolume = attrtofloat(audio_node.attribute("sfxVolume"), SoundEffectsVolume);
	}
	
	return true;
}

float ApplicationOptions::GetAspect() const {
	float sw = (float)ScreenWidth;
	float sh = (float)ScreenHeight;
	return sw / sh;
}
