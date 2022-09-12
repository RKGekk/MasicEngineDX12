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
	RootNode = RootNode.child("PlayerOptions");

	pugi::xml_node graphics_node = RootNode.child("Graphics");
	if (graphics_node) {
		DebugUI = graphics_node.child("DebugUI").text().as_bool(DebugUI);
		FullScreenMax = graphics_node.child("FullScreenMax").text().as_bool(FullScreenMax);
		FullScreen = graphics_node.child("FullScreen").text().as_bool(FullScreen);
		RunFullSpeed = graphics_node.child("RunFullSpeed").text().as_bool(RunFullSpeed);
		ScreenHeight = graphics_node.child("Height").text().as_int(ScreenHeight);
		ScreenWidth = graphics_node.child("Width").text().as_int(ScreenWidth);

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
		MusicVolume = audio_node.attribute("musicVolume").as_float(MusicVolume);
		SoundEffectsVolume = audio_node.attribute("sfxVolume").as_float(SoundEffectsVolume);
	}
	
	return true;
}

float ApplicationOptions::GetAspect() const {
	float sw = (float)ScreenWidth;
	float sh = (float)ScreenHeight;
	return sw / sh;
}
