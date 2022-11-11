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

struct KeyframeTranslation {
	KeyframeTranslation();
	~KeyframeTranslation();

	float TimePos;
	DirectX::XMFLOAT3 Translation;

	friend bool operator<(const KeyframeTranslation& kf1, const KeyframeTranslation& kf2);
};

struct KeyframeScale {
	KeyframeScale();
	~KeyframeScale();

	float TimePos;
	DirectX::XMFLOAT3 Scale;

	friend bool operator<(const KeyframeScale& kf1, const KeyframeScale& kf2);
};

struct KeyframeRotation {
	KeyframeRotation();
	~KeyframeRotation();

	float TimePos;
	DirectX::XMFLOAT4 RotationQuat;

	friend bool operator<(const KeyframeRotation& kf1, const KeyframeRotation& kf2);
};


struct BoneAnimation {
	float GetStartTime() const;
	float GetEndTime() const;

	void Interpolate(float t, DirectX::XMFLOAT4X4& M) const;

	// ASC sorted by time
	std::vector<KeyframeTranslation> TranslationKeyframes;
	std::vector<KeyframeScale> ScaleKeyframes;
	std::vector<KeyframeRotation> RotationKeyframes;
};

using BoneAnimationPtr = std::shared_ptr<BoneAnimation>;

struct AnimationClip {
	float GetClipStartTime() const;
	float GetClipEndTime() const;

	void Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& bone_transforms) const;

	std::unordered_map<std::string, BoneAnimationPtr> KeyframesMap;
	std::vector<BoneAnimationPtr> BoneAnimations;
};

struct BoneInfo {
	int id;
	DirectX::XMFLOAT4X4 offset;
};

class SkinnedData {
public:

	uint32_t BoneCount() const;

	float GetClipStartTime(const std::string& clip_name) const;
	float GetClipEndTime(const std::string& clip_name) const;

	void Set(std::vector<int>& bone_hierarchy, std::vector<DirectX::XMFLOAT4X4>& bone_offsets, std::unordered_map<std::string, AnimationClip>& animations);

	void SetBoneHierarchy(std::vector<int> bone_hierarchy);
	const std::vector<int>& GetBoneHierarchy() const;

	void SetBoneOffsets(std::vector<DirectX::XMFLOAT4X4> bone_offsets);
	const std::vector<DirectX::XMFLOAT4X4>& GetBoneOffsets() const;

	void SetBoneOffsetsMap(std::unordered_map<std::string, BoneInfo> bone_offsets_map);
	const std::unordered_map<std::string, BoneInfo>& GetBoneOffsetsMap() const;
	int CountBoneOffsetsMap(std::string name);
	void AddBoneOffsetsInfo(std::string name, BoneInfo bone_info);

	void SetAnimations(std::unordered_map<std::string, AnimationClip> animations);
	void AddAnimations(std::string name, AnimationClip animations);
	const std::unordered_map<std::string, AnimationClip>& GetAnimations() const;

	void GetFinalTransforms(const std::string& clip_name, float time_pos, std::vector<DirectX::XMFLOAT4X4>& final_transforms) const;

private:
	std::vector<int> m_bone_hierarchy;

	std::unordered_map<std::string, BoneInfo> m_bone_info_map;
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