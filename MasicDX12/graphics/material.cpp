#include "material.h"

#include "material.h"
#include "directx12_wrappers/texture.h"

#include <utility>

static MaterialProperties* NewMaterialProperties(const MaterialProperties& props) {
    MaterialProperties* material_properties = (MaterialProperties*)_aligned_malloc(sizeof(MaterialProperties), 16);
    *material_properties = props;

    return material_properties;
}

static void DeleteMaterialProperties(MaterialProperties* p) {
    _aligned_free(p);
}

Material::Material(const MaterialProperties& material_properties) : m_material_properties(NewMaterialProperties(material_properties), &DeleteMaterialProperties) {}

Material::Material(const Material& copy) : m_material_properties(NewMaterialProperties(*copy.m_material_properties), &DeleteMaterialProperties), m_textures(copy.m_textures) {}

Material& Material::operator=(const Material& right) {
    if (this == &right) {
        return *this;
    }
    m_material_properties.reset(NewMaterialProperties(*right.m_material_properties));
    m_textures = right.m_textures;
    return *this;
}

const DirectX::XMFLOAT4& Material::GetAmbientColor() const {
    return m_material_properties->Ambient;
}

void Material::SetAmbientColor(const DirectX::XMFLOAT4& ambient) {
    m_material_properties->Ambient = ambient;
}

const DirectX::XMFLOAT4& Material::GetDiffuseColor() const {
    return m_material_properties->Diffuse;
}

void Material::SetDiffuseColor(const DirectX::XMFLOAT4& diffuse) {
    m_material_properties->Diffuse = diffuse;
}

const DirectX::XMFLOAT4& Material::GetEmissiveColor() const {
    return m_material_properties->Emissive;
}

void Material::SetEmissiveColor(const DirectX::XMFLOAT4& emissive) {
    m_material_properties->Emissive = emissive;
}

const DirectX::XMFLOAT4& Material::GetSpecularColor() const {
    return m_material_properties->Specular;
}

void Material::SetSpecularColor(const DirectX::XMFLOAT4& specular) {
    m_material_properties->Specular = specular;
}

float Material::GetSpecularPower() const {
    return m_material_properties->SpecularPower;
}

void Material::SetSpecularPower(float specular_power) {
    m_material_properties->SpecularPower = specular_power;
}

const DirectX::XMFLOAT4& Material::GetReflectance() const {
    return m_material_properties->Reflectance;
}

void Material::SetReflectance(const DirectX::XMFLOAT4& reflectance) {
    m_material_properties->Reflectance = reflectance;
}

const float Material::GetOpacity() const {
    return m_material_properties->Opacity;
}

void Material::SetOpacity(float opacity) {
    m_material_properties->Opacity = opacity;
}

float Material::GetIndexOfRefraction() const {
    return m_material_properties->IndexOfRefraction;
}

void Material::SetIndexOfRefraction(float index_of_refraction) {
    m_material_properties->IndexOfRefraction = index_of_refraction;
}

float Material::GetBumpIntensity() const {
    return m_material_properties->BumpIntensity;
}

void Material::SetBumpIntensity(float bump_intensity) {
    m_material_properties->BumpIntensity = bump_intensity;
}

std::shared_ptr<Texture> Material::GetTexture(TextureType ID) const {
    TextureMap::const_iterator iter = m_textures.find(ID);
    if (iter != m_textures.end()) {
        return iter->second;
    }

    return nullptr;
}

void Material::SetTexture(TextureType type, std::shared_ptr<Texture> texture) {
    m_textures[type] = texture;
    bool has_texture = (texture != nullptr);
    uint32_t texture_type_bit = 0u;
    switch (type) {
        case TextureType::Ambient: {
            if (has_texture) m_material_properties->HasTexture |= HAS_AMBIENT_TEXTURE;
            else m_material_properties->HasTexture &= !HAS_AMBIENT_TEXTURE;
        }
        break;
        case TextureType::Emissive: {
            if (has_texture) m_material_properties->HasTexture |= HAS_EMISSIVE_TEXTURE;
            else m_material_properties->HasTexture &= !HAS_EMISSIVE_TEXTURE;
        }
        break;
        case TextureType::Diffuse: {
            if (has_texture) m_material_properties->HasTexture |= HAS_DIFFUSE_TEXTURE;
            else m_material_properties->HasTexture &= !HAS_DIFFUSE_TEXTURE;
        }
        break;
        case TextureType::Specular: {
            if (has_texture) m_material_properties->HasTexture |= HAS_SPECULAR_TEXTURE;
            else m_material_properties->HasTexture &= !HAS_SPECULAR_TEXTURE;
        }
        break;
        case TextureType::SpecularPower: {
            if (has_texture) m_material_properties->HasTexture |= HAS_SPECULAR_POWER_TEXTURE;
            else m_material_properties->HasTexture &= !HAS_SPECULAR_POWER_TEXTURE;
        }
        break;
        case TextureType::Normal: {
            if (has_texture) m_material_properties->HasTexture |= HAS_NORMAL_TEXTURE;
            else m_material_properties->HasTexture &= !HAS_NORMAL_TEXTURE;
        }
        break;
        case TextureType::Bump: {
            if (has_texture) m_material_properties->HasTexture |= HAS_BUMP_TEXTURE;
            else m_material_properties->HasTexture &= !HAS_BUMP_TEXTURE;
        }
        break;
        case TextureType::Opacity: {
            if (has_texture) m_material_properties->HasTexture |= HAS_OPACITY_TEXTURE;
            else m_material_properties->HasTexture &= !HAS_OPACITY_TEXTURE;
        }
        break;
        case TextureType::Displacement: {
            if (has_texture) m_material_properties->HasTexture |= HAS_DISPLACEMENT_TEXTURE;
            else m_material_properties->HasTexture &= !HAS_DISPLACEMENT_TEXTURE;
        }
        break;
        case TextureType::Metalness: {
            if (has_texture) m_material_properties->HasTexture |= HAS_METALNESS_TEXTURE;
            else m_material_properties->HasTexture &= !HAS_METALNESS_TEXTURE;
        }
        break;
        case TextureType::Shadow: {
            if (has_texture) m_material_properties->HasTexture |= HAS_SHADOW_TEXTURE;
            else m_material_properties->HasTexture &= !HAS_SHADOW_TEXTURE;
        }
        break;
    }
}

void Material::SetInvYNormalTextureFlag(bool is_inv_y_texture) {
    if(is_inv_y_texture) m_material_properties->HasTexture |= HAS_NORMAL_INV_Y_TEXTURE;
    else m_material_properties->HasTexture *= ~HAS_NORMAL_INV_Y_TEXTURE;
}

bool Material::IsTransparent() const {
    return (m_material_properties->Opacity < 1.0f || (m_material_properties->HasTexture & HAS_OPACITY_TEXTURE));
}

const MaterialProperties& Material::GetMaterialProperties() const {
    return *m_material_properties;
}

void Material::SetMaterialProperties(const MaterialProperties& material_properties) {
    *m_material_properties = material_properties;
}

const std::string& Material::GetName() const {
    return m_name;
}

void Material::SetName(std::string name) {
    m_name = std::move(name);
}

const MaterialProperties Material::Zero = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    0.0f,
    { 0.0f, 0.0f, 0.0f, 1.0f }
};

const MaterialProperties Material::Red = {
    { 1.0f, 0.0f, 0.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
    128.0f,
    { 0.1f, 0.0f, 0.0f, 1.0f }
};

const MaterialProperties Material::Green = {
    { 0.0f, 1.0f, 0.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
    128.0f,
    { 0.0f, 0.1f, 0.0f, 1.0f }
};

const MaterialProperties Material::Blue = {
    { 0.0f, 0.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
    128.0f,
    { 0.0f, 0.0f, 0.1f, 1.0f }
};

const MaterialProperties Material::Cyan = {
    { 0.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
    128.0f,
    { 0.0f, 0.1f, 0.1f, 1.0f }
};

const MaterialProperties Material::Magenta = {
    { 1.0f, 0.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
    128.0f,
    { 0.1f, 0.0f, 0.1f, 1.0f }
};

const MaterialProperties Material::Yellow = {
    { 0.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
    128.0f,
    { 0.0f, 0.1f, 0.1f, 1.0f }
};

const MaterialProperties Material::White = {
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
    128.0f,
    { 0.1f, 0.1f, 0.1f, 1.0f }
};

const MaterialProperties Material::WhiteDiffuse = {
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    0.0f,
    { 0.0f, 0.0f, 0.0f, 1.0f }
};

const MaterialProperties Material::Black = {
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    0.0f,
    { 0.0f, 0.0f, 0.0f, 1.0f }
};

const MaterialProperties Material::Emerald = {
    { 0.07568f, 0.61424f, 0.07568f, 1.0f },
    { 0.633f, 0.727811f, 0.633f, 1.0f },
    76.8f,
    { 0.0215f, 0.1745f, 0.0215f, 1.0f }
};

const MaterialProperties Material::Jade = {
    { 0.54f, 0.89f, 0.63f, 1.0f },
    { 0.316228f, 0.316228f, 0.316228f, 1.0f },
    12.8f,
    { 0.135f, 0.2225f, 0.1575f, 1.0f }
};

const MaterialProperties Material::Obsidian = {
    { 0.18275f, 0.17f, 0.22525f, 1.0f },
    { 0.332741f, 0.328634f, 0.346435f, 1.0f },
    38.4f,
    { 0.05375f, 0.05f, 0.06625f, 1.0f }
};

const MaterialProperties Material::Pearl = {
    { 1.0f, 0.829f, 0.829f, 1.0f },
    { 0.296648f, 0.296648f, 0.296648f, 1.0f },
    11.264f,
    { 0.25f, 0.20725f, 0.20725f, 1.0f }
};

const MaterialProperties Material::Ruby = {
    { 0.61424f, 0.04136f, 0.04136f, 1.0f },
    { 0.727811f, 0.626959f, 0.626959f, 1.0f },
    76.8f,
    { 0.1745f, 0.01175f, 0.01175f, 1.0f }
};

const MaterialProperties Material::Turquoise = {
    { 0.396f, 0.74151f, 0.69102f, 1.0f },
    { 0.297254f, 0.30829f, 0.306678f, 1.0f },
    12.8f,
    { 0.1f, 0.18725f, 0.1745f, 1.0f }
};

const MaterialProperties Material::Brass = {
    { 0.780392f, 0.568627f, 0.113725f, 1.0f },
    { 0.992157f, 0.941176f, 0.807843f, 1.0f },
    27.9f,
    { 0.329412f, 0.223529f, 0.027451f, 1.0f }
};

const MaterialProperties Material::Bronze = {
    { 0.714f, 0.4284f, 0.18144f, 1.0f },
    { 0.393548f, 0.271906f, 0.166721f, 1.0f },
    25.6f,
    { 0.2125f, 0.1275f, 0.054f, 1.0f }
};

const MaterialProperties Material::Chrome = {
    { 0.4f, 0.4f, 0.4f, 1.0f },
    { 0.774597f, 0.774597f, 0.774597f, 1.0f },
    76.8f,
    { 0.25f, 0.25f, 0.25f, 1.0f }
};

const MaterialProperties Material::Copper = {
    { 0.7038f, 0.27048f, 0.0828f, 1.0f },
    { 0.256777f, 0.137622f, 0.086014f, 1.0f },
    12.8f,
    { 0.19125f, 0.0735f, 0.0225f, 1.0f }
};

const MaterialProperties Material::Gold = {
    { 0.75164f, 0.60648f, 0.22648f, 1.0f },
    { 0.628281f, 0.555802f, 0.366065f, 1.0f },
    51.2f,
    { 0.24725f, 0.1995f, 0.0745f, 1.0f }
};

const MaterialProperties Material::Silver = {
    { 0.50754f, 0.50754f, 0.50754f, 1.0f },
    { 0.508273f, 0.508273f, 0.508273f, 1.0f },
    51.2f,
    { 0.19225f, 0.19225f, 0.19225f, 1.0f }
};

const MaterialProperties Material::BlackPlastic = {
    { 0.01f, 0.01f, 0.01f, 1.0f },
    { 0.5f, 0.5f, 0.5f, 1.0f },
    32.0f,
    { 0.0f, 0.0f, 0.0f, 1.0f }
};

const MaterialProperties Material::CyanPlastic = {
    { 0.0f, 0.50980392f, 0.50980392f, 1.0f },
    { 0.50196078f, 0.50196078f, 0.50196078f, 1.0f },
    32.0f,
    { 0.0f, 0.1f, 0.06f, 1.0f }
};

const MaterialProperties Material::GreenPlastic = {
    { 0.1f, 0.35f, 0.1f, 1.0f },
    { 0.45f, 0.55f, 0.45f, 1.0f },
    32.0f,
    { 0.0f, 0.0f, 0.0f, 1.0f }
};

const MaterialProperties Material::RedPlastic = {
    { 0.5f, 0.0f, 0.0f, 1.0f },
    { 0.7f, 0.6f, 0.6f, 1.0f },
    32.0f,
    { 0.0f, 0.0f, 0.0f, 1.0f }
};

const MaterialProperties Material::WhitePlastic = {
    { 0.55f, 0.55f, 0.55f, 1.0f },
    { 0.7f, 0.7f, 0.7f, 1.0f },
    32.0f,
    { 0.0f, 0.0f, 0.0f, 1.0f }
};

const MaterialProperties Material::YellowPlastic = {
    { 0.5f, 0.5f, 0.0f, 1.0f },
    { 0.6f, 0.6f, 0.5f, 1.0f },
    32.0f,
    { 0.0f, 0.0f, 0.0f, 1.0f }
};

const MaterialProperties Material::BlackRubber = {
    { 0.01f, 0.01f, 0.01f, 1.0f },
    { 0.4f, 0.4f, 0.4f, 1.0f },
    10.0f,
    { 0.02f, 0.02f, 0.02f, 1.0f }
};

const MaterialProperties Material::CyanRubber = {
    { 0.4f, 0.5f, 0.5f, 1.0f },
    { 0.04f, 0.7f, 0.7f, 1.0f },
    10.0f,
    { 0.0f, 0.05f, 0.05f, 1.0f }
};

const MaterialProperties Material::GreenRubber = {
    { 0.4f, 0.5f, 0.4f, 1.0f },
    { 0.04f, 0.7f, 0.04f, 1.0f },
    10.0f,
    { 0.0f, 0.05f, 0.0f, 1.0f }
};

const MaterialProperties Material::RedRubber = {
    { 0.5f, 0.4f, 0.4f, 1.0f },
    { 0.7f, 0.04f, 0.04f, 1.0f },
    10.0f,
    { 0.05f, 0.0f, 0.0f, 1.0f }
};

const MaterialProperties Material::WhiteRubber = {
    { 0.5f, 0.5f, 0.5f, 1.0f },
    { 0.7f, 0.7f, 0.7f, 1.0f },
    10.0f,
    { 0.05f, 0.05f, 0.05f, 1.0f }
};

const MaterialProperties Material::YellowRubber = {
    { 0.5f, 0.5f, 0.4f, 1.0f },
    { 0.7f, 0.7f, 0.04f, 1.0f },
    10.0f,
    { 0.05f, 0.05f, 0.0f, 1.0f }
};