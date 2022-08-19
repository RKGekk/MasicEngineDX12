#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <d3d12.h>

class InputAssemblerLayout {
public:
    InputAssemblerLayout() = default;
    InputAssemblerLayout(const D3D12_INPUT_LAYOUT_DESC& desc);

    void SetInputAssemblerLayout(const D3D12_INPUT_LAYOUT_DESC& desc);
    void AddPerVertexLayoutElement(const std::string& semantic_name, uint32_t semantic_index, DXGI_FORMAT format, uint32_t input_slot, uint32_t alighned_byte_offset);
    void AddPerInstanceLayoutElement(const std::string& semantic_name, uint32_t semantic_index, DXGI_FORMAT format, uint32_t input_slot, uint32_t alighned_byte_offset, uint32_t step_rate);

    const D3D12_INPUT_LAYOUT_DESC& GetLayout() const;

private:
    void SetSemanticNames();

    std::vector<std::string> m_element_semantic_names;
    std::vector<D3D12_INPUT_ELEMENT_DESC> m_input_elements;
    D3D12_INPUT_LAYOUT_DESC m_desc{};
};