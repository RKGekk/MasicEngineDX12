#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>

#include <DirectXMath.h>

struct Keyframe {
	Keyframe();
	~Keyframe();

	float TimePos;
	DirectX::XMFLOAT3 Translation;
	DirectX::XMFLOAT3 Scale;
	DirectX::XMFLOAT4 RotationQuat;
};

struct BoneAnimation {
	float GetStartTime() const;
	float GetEndTime() const;

	void Interpolate(float t, DirectX::XMFLOAT4X4& M) const;

	std::vector<Keyframe> Keyframes;
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