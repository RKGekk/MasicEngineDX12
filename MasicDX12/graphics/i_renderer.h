#pragma once

#include <memory>

#include <Windows.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include "../window_surface.h"

class IRenderer {
public:
	virtual bool Initialize(std::shared_ptr<WindowSurface> rw) = 0;

	virtual HRESULT VOnRestore() = 0;
	virtual void VShutdown() = 0;
	virtual bool VPreRender() = 0;
	virtual bool VPresent() = 0;
	virtual bool VPostRender() = 0;

	virtual std::shared_ptr<WindowSurface> GetRenderWindow() = 0;
};