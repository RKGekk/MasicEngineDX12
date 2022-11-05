#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include <DirectXMath.h>

constexpr auto MAX_BONE_TRANSFORMS = 96;

struct SkinnedConstants {
	DirectX::XMFLOAT4X4 BoneTransforms[MAX_BONE_TRANSFORMS];
};

struct Keyframe {
	Keyframe();
	~Keyframe();

	float TimePos;
	DirectX::XMFLOAT3 Translation;
	DirectX::XMFLOAT3 Scale;
	DirectX::XMFLOAT4 RotationQuat;

	friend bool operator<(const Keyframe& kf1, const Keyframe& kf2);
};

struct BoneAnimation {
	float GetStartTime() const;
	float GetEndTime() const;

	void Interpolate(float t, DirectX::XMFLOAT4X4& M) const;

	std::vector<Keyframe> Keyframes; // ASC sorted by time
};

struct AnimationClip {
	float GetClipStartTime() const;
	float GetClipEndTime() const;

	void Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& bone_transforms) const;

	std::vector<BoneAnimation> BoneAnimations;
};

class SkinnedData {
public:

	uint32_t BoneCount() const;

	float GetClipStartTime(const std::string& clip_name) const;
	float GetClipEndTime(const std::string& clip_name) const;

	void Set(std::vector<int>& bone_hierarchy, std::vector<DirectX::XMFLOAT4X4>& bone_offsets, std::unordered_map<std::string, AnimationClip>& animations);

	void GetFinalTransforms(const std::string& clip_name, float time_pos, std::vector<DirectX::XMFLOAT4X4>& final_transforms) const;

private:
	std::vector<int> m_bone_hierarchy;
	std::vector<DirectX::XMFLOAT4X4> m_bone_offsets;

	std::unordered_map<std::string, AnimationClip> m_animations;
};

struct SkinnedModelInstance {
	std::shared_ptr<SkinnedData> SkinnedInfo = nullptr;
	std::vector<DirectX::XMFLOAT4X4> FinalTransforms;
	std::string ClipName;
	float TimePos = 0.0f;

	void UpdateSkinnedAnimation(float dt);
};