#include "input_assembler_layout.h"

InputAssemblerLayout::InputAssemblerLayout(const D3D12_INPUT_LAYOUT_DESC& desc) {
    SetInputAssemblerLayout(desc);
}

void InputAssemblerLayout::SetInputAssemblerLayout(const D3D12_INPUT_LAYOUT_DESC& desc) {
    m_input_elements.clear();
    m_element_semantic_names.clear();

    size_t sz = desc.NumElements;
    for (int i = 0; i < sz; ++i) {
        D3D12_INPUT_ELEMENT_DESC element = desc.pInputElementDescs[i];
        m_input_elements.push_back(element);
        m_element_semantic_names.push_back(element.SemanticName);
    }

    m_desc.NumElements = (UINT)m_input_elements.size();
    m_desc.pInputElementDescs = m_input_elements.data();
}

void InputAssemblerLayout::AddPerVertexLayoutElement(const std::string& semantic_name, uint32_t semantic_index, DXGI_FORMAT format, uint32_t input_slot, uint32_t alighned_byte_offset) {
    D3D12_INPUT_ELEMENT_DESC desc{};
    desc.Format = format;
    desc.AlignedByteOffset = alighned_byte_offset;
    desc.InputSlot = input_slot;
    desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
    desc.SemanticIndex = semantic_index;

    m_input_elements.push_back(desc);
    m_element_semantic_names.push_back(semantic_name);

    m_desc.NumElements = (UINT)m_input_elements.size();
    m_desc.pInputElementDescs = m_input_elements.data();

    SetSemanticNames();
}

void InputAssemblerLayout::AddPerInstanceLayoutElement(const std::string& semantic_name, uint32_t semantic_index, DXGI_FORMAT format, uint32_t input_slot, uint32_t alighned_byte_offset, uint32_t step_rate) {
    D3D12_INPUT_ELEMENT_DESC desc{};
    desc.Format = format;
    desc.AlignedByteOffset = alighned_byte_offset;
    desc.InputSlot = input_slot;
    desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
    desc.SemanticIndex = semantic_index;
    desc.InstanceDataStepRate = step_rate;

    m_input_elements.push_back(desc);
    m_element_semantic_names.push_back(semantic_name);

    m_desc.NumElements = (UINT)m_input_elements.size();
    m_desc.pInputElementDescs = m_input_elements.data();

    SetSemanticNames();
}

const D3D12_INPUT_LAYOUT_DESC& InputAssemblerLayout::GetLayout() const {
    return m_desc;
}

void InputAssemblerLayout::SetSemanticNames() {
    size_t sz = m_input_elements.size();
    for (auto i = 0; i < sz; i++) {
        m_input_elements[i].SemanticName = m_element_semantic_names[i].c_str();
    }
}