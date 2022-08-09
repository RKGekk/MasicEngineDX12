#pragma once

#include <string>

#include <DirectXMath.h>
#include <pugixml/pugixml.hpp>

#include "engine/renderer_enum.h"

struct ApplicationOptions {
	Renderer Renderer = Renderer::Renderer_D3D12;
	bool RunFullSpeed = false;
	bool FullScreen = false;
	bool FullScreenMax = true;
	bool ScreenTearing = false;
	int ScreenWidth = 800;
	int ScreenHeight = 600;
	bool DebugUI = true;

	float SoundEffectsVolume = 1.0f;
	float MusicVolume = 1.0f;

	pugi::xml_node RootNode;

	ApplicationOptions();
	ApplicationOptions(const std::string& xmlFilePath);
	~ApplicationOptions();

	bool Init(const std::string& xmlFilePath);

	float GetAspect() const;
};
