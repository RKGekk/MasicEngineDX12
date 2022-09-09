#pragma once

#include <../graphics/imgui/imgui.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <memory>

class CommandList;
class Device;
class PipelineStateObject;
class RenderTarget;
class RootSignature;
class ShaderResourceView;
class Texture;
class VertexShader;
class PixelShader;

class GUI {
public:
	LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void NewFrame();
	void Render(const std::shared_ptr<CommandList>& commandList, const RenderTarget& renderTarget);
	void Destroy();
	void SetScaling(float scale);

protected:
	GUI(Device& device, HWND hWnd, const RenderTarget& render_target);
	virtual ~GUI();

private:
	Device& m_device;
	HWND m_hWnd;
	ImGuiContext* m_pImgui_ctx;
	std::shared_ptr<Texture> m_font_texture;
	std::shared_ptr<ShaderResourceView> m_font_srv;
	std::shared_ptr<RootSignature> m_root_signature;
	std::shared_ptr<PipelineStateObject> m_pipeline_state;
	std::shared_ptr<VertexShader> m_vertex_shader;
	std::shared_ptr<PixelShader> m_pixel_shader;
};