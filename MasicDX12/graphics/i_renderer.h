#pragma once

#include <memory>

#include <Windows.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include "../window_surface.h"

class IRenderer {
public:
	virtual bool Initialize(const WindowSurface& rw) = 0;

	virtual HRESULT VOnRestore(const WindowSurface& rw) = 0;
	virtual void VShutdown() = 0;
	virtual bool VPreRender() = 0;
	virtual bool VPresent() = 0;
	virtual bool VPostRender() = 0;
};