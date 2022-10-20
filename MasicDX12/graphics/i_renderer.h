#pragma once

#include <memory>

#include <Windows.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include "../window_surface.h"

class CommandList;
class RenderTarget;

class IRenderer {
public:
	virtual bool Initialize(std::shared_ptr<WindowSurface> rw) = 0;

	virtual void VSetClearColor(BYTE R, BYTE G, BYTE B, BYTE A) = 0;
	virtual void VSetClearColor4f(float R, float G, float B, float A) = 0;
	virtual DirectX::XMFLOAT4 VGetClearColor4f() = 0;

	virtual HRESULT VOnRestore() = 0;
	virtual void VShutdown() = 0;
	virtual bool VPreRender(std::shared_ptr<CommandList> command_list) = 0;
	virtual bool VPresent() = 0;
	virtual bool VPostRender() = 0;

	virtual std::shared_ptr<WindowSurface> GetRenderWindow() = 0;
	virtual RenderTarget& GetRenderTarget() = 0;
	virtual uint32_t GetRenderTargetWidth() = 0;
	virtual uint32_t GetRenderTargetHeight() = 0;
};