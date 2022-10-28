#include "skinned_data.h"

#include "../tools/math_utitity.h"

#include <algorithm>
	
Keyframe::Keyframe() : TimePos(0.0f), Translation(0.0f, 0.0f, 0.0f), Scale(1.0f, 1.0f, 1.0f), RotationQuat(0.0f, 0.0f, 0.0f, 1.0f) {}

Keyframe::~Keyframe() {}

float BoneAnimation::GetStartTime() const {
	return Keyframes.front().TimePos;
}

float BoneAnimation::GetEndTime() const {
	float f = Keyframes.back().TimePos;
	return f;
}

void BoneAnimation::Interpolate(float t, DirectX::XMFLOAT4X4& M) const {
	using namespace DirectX;
	if (t <= Keyframes.front().TimePos) {
		XMVECTOR S = XMLoadFloat3(&Keyframes.front().Scale);
		XMVECTOR P = XMLoadFloat3(&Keyframes.front().Translation);
		XMVECTOR Q = XMLoadFloat4(&Keyframes.front().RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else if (t >= Keyframes.back().TimePos) {
		XMVECTOR S = XMLoadFloat3(&Keyframes.back().Scale);
		XMVECTOR P = XMLoadFloat3(&Keyframes.back().Translation);
		XMVECTOR Q = XMLoadFloat4(&Keyframes.back().RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else {
		for (uint32_t i = 0; i < Keyframes.size() - 1; ++i) {
			if (t >= Keyframes[i].TimePos && t <= Keyframes[i + 1].TimePos) {
				float lerp_percent = (t - Keyframes[i].TimePos) / (Keyframes[i + 1].TimePos - Keyframes[i].TimePos);

				XMVECTOR s0 = XMLoadFloat3(&Keyframes[i].Scale);
				XMVECTOR s1 = XMLoadFloat3(&Keyframes[i + 1].Scale);

				XMVECTOR p0 = XMLoadFloat3(&Keyframes[i].Translation);
				XMVECTOR p1 = XMLoadFloat3(&Keyframes[i + 1].Translation);

				XMVECTOR q0 = XMLoadFloat4(&Keyframes[i].RotationQuat);
				XMVECTOR q1 = XMLoadFloat4(&Keyframes[i + 1].RotationQuat);

				XMVECTOR S = XMVectorLerp(s0, s1, lerp_percent);
				XMVECTOR P = XMVectorLerp(p0, p1, lerp_percent);
				XMVECTOR Q = XMQuaternionSlerp(q0, q1, lerp_percent);

				XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
				XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));

				break;
			}
		}
	}
}

float AnimationClip::GetClipStartTime() const {
	float t = std::numeric_limits<float>::infinity();
	for (uint32_t i = 0; i < BoneAnimations.size(); ++i) {
		t = std::min(t, BoneAnimations[i].GetStartTime());
	}

	return t;
}

float AnimationClip::GetClipEndTime() const {
	float t = 0.0f;
	for (uint32_t i = 0; i < BoneAnimations.size(); ++i) {
		t = std::max(t, BoneAnimations[i].GetEndTime());
	}

	return t;
}

void AnimationClip::Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& bone_transforms) const {
	for (uint32_t i = 0; i < BoneAnimations.size(); ++i) {
		BoneAnimations[i].Interpolate(t, bone_transforms[i]);
	}
}

uint32_t SkinnedData::BoneCount() const {
	return m_bone_hierarchy.size();
}

float SkinnedData::GetClipStartTime(const std::string& clip_name) const {
	auto clip = m_animations.find(clip_name);
	return clip->second.GetClipStartTime();
}

float SkinnedData::GetClipEndTime(const std::string& clip_name) const {
	auto clip = m_animations.find(clip_name);
	return clip->second.GetClipEndTime();
}

void SkinnedData::Set(std::vector<int>& bone_hierarchy, std::vector<DirectX::XMFLOAT4X4>& bone_offsets, std::unordered_map<std::string, AnimationClip>& animations) {
	m_bone_hierarchy = bone_hierarchy;
	m_bone_offsets = bone_offsets;
	m_animations = animations;
}

void SkinnedData::GetFinalTransforms(const std::string& clip_name, float time_pos, std::vector<DirectX::XMFLOAT4X4>& final_transforms) const {
	using namespace DirectX;
	uint32_t num_bones = m_bone_offsets.size();

	std::vector<XMFLOAT4X4> to_parent_transforms(num_bones);

	auto clip = m_animations.find(clip_name);
	clip->second.Interpolate(time_pos, to_parent_transforms);

	std::vector<XMFLOAT4X4> to_root_transforms(num_bones);

	to_root_transforms[0] = to_parent_transforms[0];

	for (uint32_t i = 1; i < num_bones; ++i) {
		XMMATRIX to_parent = XMLoadFloat4x4(&to_parent_transforms[i]);

		int parent_index = m_bone_hierarchy[i];
		XMMATRIX parent_to_root = XMLoadFloat4x4(&to_root_transforms[parent_index]);

		XMMATRIX to_root = XMMatrixMultiply(to_parent, parent_to_root);

		XMStoreFloat4x4(&to_root_transforms[i], to_root);
	}

	for (uint32_t i = 0; i < num_bones; ++i) {
		XMMATRIX offset = XMLoadFloat4x4(&m_bone_offsets[i]);
		XMMATRIX to_root = XMLoadFloat4x4(&to_root_transforms[i]);
		XMMATRIX final_transform = XMMatrixMultiply(offset, to_root);
		XMStoreFloat4x4(&final_transforms[i], XMMatrixTranspose(final_transform));
	}
}
