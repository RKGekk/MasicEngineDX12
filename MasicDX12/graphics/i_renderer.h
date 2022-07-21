#pragma once

#include <memory>

#include <Windows.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include "../engine/render_window.h"

class IRenderer {
public:
	virtual bool Initialize(const RenderWindow& rw) = 0;

	virtual void VSetBackgroundColor(DirectX::PackedVector::XMCOLOR color) = 0;
	virtual void VSetBackgroundColor4f(const DirectX::XMFLOAT4& color) = 0;
	virtual void VSetBackgroundColor3f(const DirectX::XMFLOAT3& color) = 0;
	virtual void VSetBackgroundColor(DirectX::FXMVECTOR color) = 0;

	virtual HRESULT VOnRestore(const RenderWindow& rw) = 0;
	virtual void VShutdown() = 0;
	virtual bool VPreRender() = 0;
	virtual bool VPresent() = 0;
	virtual bool VPostRender() = 0;
};