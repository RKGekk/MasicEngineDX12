#pragma once

#include <DirectXMath.h>

#include <map>
#include <memory>

class Texture;

struct alignas(16) MaterialProperties {

	// The Material properties must be aligned to a 16-byte boundary.
	// To guarantee alignment, the MaterialProperties structure will be allocated in aligned memory.
	MaterialProperties(
		const DirectX::XMFLOAT4 diffuse = { 1, 1, 1, 1 },
		const DirectX::XMFLOAT4 specular = { 1, 1, 1, 1 },
		const float specularPower = 128.0f,
		const DirectX::XMFLOAT4 ambient = { 0, 0, 0, 1 },
		const DirectX::XMFLOAT4 emissive = { 0, 0, 0, 1 },
		const DirectX::XMFLOAT4 reflectance = { 0, 0, 0, 0 },
		const float opacity = 1.0f,
		const float indexOfRefraction = 0.0f,
		const float bumpIntensity = 1.0f,
		const float alphaThreshold = 0.1f
	)
		: Diffuse(diffuse)
		, Specular(specular)
		, Emissive(emissive)
		, Ambient(ambient)
		, Reflectance(reflectance)
		, Opacity(opacity)
		, SpecularPower(specularPower)
		, IndexOfRefraction(indexOfRefraction)
		, BumpIntensity(bumpIntensity)
		, HasAmbientTexture(false)
		, HasEmissiveTexture(false)
		, HasDiffuseTexture(false)
		, HasSpecularTexture(false)
		, HasSpecularPowerTexture(false)
		, HasNormalTexture(false)
		, HasBumpTexture(false)
		, HasOpacityTexture(false) {}

	DirectX::XMFLOAT4 Diffuse;
	//1------------------------------------ ( 16 bytes )

	DirectX::XMFLOAT4 Specular;
	//2------------------------------------ ( 16 bytes )

	DirectX::XMFLOAT4 Emissive;
	//2------------------------------------ ( 16 bytes )

	DirectX::XMFLOAT4 Ambient;
	//4------------------------------------ ( 16 bytes )

	DirectX::XMFLOAT4 Reflectance;
	//5------------------------------------ ( 16 bytes )

	float Opacity;				// If Opacity < 1, then the material is transparent.
	float SpecularPower;
	float IndexOfRefraction;	// For transparent materials, IOR > 0.
	float BumpIntensity;		// When using bump textures (height maps) we need to scale the height values so the normals are visible.
	//6------------------------------------ ( 16 bytes )

	uint32_t HasAmbientTexture;
	uint32_t HasEmissiveTexture;
	uint32_t HasDiffuseTexture;
	uint32_t HasSpecularTexture;
	//7------------------------------------ ( 16 bytes )

	uint32_t HasSpecularPowerTexture;
	uint32_t HasNormalTexture;
	uint32_t HasBumpTexture;
	uint32_t HasOpacityTexture;
	//8------------------------------------ ( 16 bytes )
	//Total:                                ( 16 * 8 = 128 bytes )
};

class Material {
public:

	enum class TextureType {
		Ambient,
		Emissive,
		Diffuse,
		Specular,
		SpecularPower,
		Normal,
		Bump,
		Opacity,
		NumTypes,
	};

	Material(const MaterialProperties& material_properties = MaterialProperties());
	Material(const Material& copy);

	~Material() = default;

	Material& operator=(const Material& right);

	const DirectX::XMFLOAT4& GetAmbientColor() const;
	void SetAmbientColor(const DirectX::XMFLOAT4& ambient);

	const DirectX::XMFLOAT4& GetDiffuseColor() const;
	void SetDiffuseColor(const DirectX::XMFLOAT4& diffuse);

	const DirectX::XMFLOAT4& GetEmissiveColor() const;
	void SetEmissiveColor(const DirectX::XMFLOAT4& emissive);

	const DirectX::XMFLOAT4& GetSpecularColor() const;
	void SetSpecularColor(const DirectX::XMFLOAT4& specular);

	float GetSpecularPower() const;
	void  SetSpecularPower(float specular_power);

	const DirectX::XMFLOAT4& GetReflectance() const;
	void SetReflectance(const DirectX::XMFLOAT4& reflectance);

	const float GetOpacity() const;
	void SetOpacity(float opacity);

	float GetIndexOfRefraction() const;
	void SetIndexOfRefraction(float index_of_refraction);

	float GetBumpIntensity() const;
	void  SetBumpIntensity(float bump_intensity);

	std::shared_ptr<Texture> GetTexture(TextureType ID) const;
	void SetTexture(TextureType type, std::shared_ptr<Texture> texture);

	bool IsTransparent() const;

	const MaterialProperties& GetMaterialProperties() const;
	void SetMaterialProperties(const MaterialProperties& material_properties);

	static const MaterialProperties Zero;
	static const MaterialProperties Red;
	static const MaterialProperties Green;
	static const MaterialProperties Blue;
	static const MaterialProperties Cyan;
	static const MaterialProperties Magenta;
	static const MaterialProperties Yellow;
	static const MaterialProperties White;
	static const MaterialProperties WhiteDiffuse;
	static const MaterialProperties Black;
	static const MaterialProperties Emerald;
	static const MaterialProperties Jade;
	static const MaterialProperties Obsidian;
	static const MaterialProperties Pearl;
	static const MaterialProperties Ruby;
	static const MaterialProperties Turquoise;
	static const MaterialProperties Brass;
	static const MaterialProperties Bronze;
	static const MaterialProperties Chrome;
	static const MaterialProperties Copper;
	static const MaterialProperties Gold;
	static const MaterialProperties Silver;
	static const MaterialProperties BlackPlastic;
	static const MaterialProperties CyanPlastic;
	static const MaterialProperties GreenPlastic;
	static const MaterialProperties RedPlastic;
	static const MaterialProperties WhitePlastic;
	static const MaterialProperties YellowPlastic;
	static const MaterialProperties BlackRubber;
	static const MaterialProperties CyanRubber;
	static const MaterialProperties GreenRubber;
	static const MaterialProperties RedRubber;
	static const MaterialProperties WhiteRubber;
	static const MaterialProperties YellowRubber;

protected:
private:
	using TextureMap = std::map<TextureType, std::shared_ptr<Texture>>;
	using MaterialPropertiesPtr = std::unique_ptr<MaterialProperties, void (*)(MaterialProperties*)>;

	MaterialPropertiesPtr m_material_properties;
	TextureMap m_textures;
};
