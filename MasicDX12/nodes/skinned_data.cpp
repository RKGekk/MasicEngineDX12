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
	if (!Keyframes.size()) {
		DirectX::XMStoreFloat4x4(&M, DirectX::XMMatrixIdentity());
		return;
	}
	if ((Keyframes.size() == 1) || (t <= Keyframes.begin()->TimePos)) {
		auto begin_it = Keyframes.begin();
		XMVECTOR S = XMLoadFloat3(&begin_it->Scale);
		XMVECTOR P = XMLoadFloat3(&begin_it->Translation);
		XMVECTOR Q = XMLoadFloat4(&begin_it->RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else if (t >= Keyframes.rbegin()->TimePos) {
		auto last_it = Keyframes.begin();
		XMVECTOR S = XMLoadFloat3(&last_it->Scale);
		XMVECTOR P = XMLoadFloat3(&last_it->Translation);
		XMVECTOR Q = XMLoadFloat4(&last_it->RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else {
		auto it1 = std::lower_bound(Keyframes.cbegin(), Keyframes.cend(), t, [](const auto it, float t) { return it.TimePos < t; });
		auto it0 = std::prev(it1);

		float current_time_pos = it0->TimePos;
		float next_time_pos = it1->TimePos;
		float lerp_percent = (t - current_time_pos) / (next_time_pos - current_time_pos);

		XMVECTOR s0 = XMLoadFloat3(&it0->Scale);
		XMVECTOR s1 = XMLoadFloat3(&it1->Scale);

		XMVECTOR p0 = XMLoadFloat3(&it0->Translation);
		XMVECTOR p1 = XMLoadFloat3(&it1->Translation);

		XMVECTOR q0 = XMLoadFloat4(&it0->RotationQuat);
		XMVECTOR q1 = XMLoadFloat4(&it1->RotationQuat);

		XMVECTOR S = XMVectorLerp(s0, s1, lerp_percent);
		XMVECTOR P = XMVectorLerp(p0, p1, lerp_percent);
		XMVECTOR Q = XMQuaternionSlerp(q0, q1, lerp_percent);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
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

	// Interpolate all the bones of this clip at the given time instance.
	auto clip = m_animations.find(clip_name);
	clip->second.Interpolate(time_pos, to_parent_transforms);

	// Traverse the hierarchy and transform all the bones to the root space.
	std::vector<XMFLOAT4X4> to_root_transforms(num_bones);

	// The root bone has index 0. The root bone has no parent, so its toRootTransform is just its local bone transform.
	to_root_transforms[0] = to_parent_transforms[0];

	// Now find the toRootTransform of the children.
	for (uint32_t i = 1u; i < num_bones; ++i) {
		XMMATRIX to_parent = XMLoadFloat4x4(&to_parent_transforms[i]);

		int parent_index = m_bone_hierarchy[i];
		XMMATRIX parent_to_root = XMLoadFloat4x4(&to_root_transforms[parent_index]);

		XMMATRIX to_root = XMMatrixMultiply(to_parent, parent_to_root);

		XMStoreFloat4x4(&to_root_transforms[i], to_root);
	}

	// Premultiply by the bone offset transform to get the final transform.
	for (uint32_t i = 0; i < num_bones; ++i) {
		XMMATRIX offset = XMLoadFloat4x4(&m_bone_offsets[i]);
		XMMATRIX to_root = XMLoadFloat4x4(&to_root_transforms[i]);
		XMMATRIX final_transform = XMMatrixMultiply(offset, to_root);
		XMStoreFloat4x4(&final_transforms[i], XMMatrixTranspose(final_transform));
	}
}

void SkinnedModelInstance::UpdateSkinnedAnimation(float dt) {
	TimePos += dt;
	if (TimePos > SkinnedInfo->GetClipEndTime(ClipName)) {
		TimePos = 0.0f;
	}
	SkinnedInfo->GetFinalTransforms(ClipName, TimePos, FinalTransforms);
}

bool operator<(const Keyframe& kf1, const Keyframe& kf2) {
	return kf1.TimePos < kf2.TimePos;
}