#include "skinned_data.h"

#include "../tools/math_utitity.h"

#include <algorithm>

KeyframeTranslation::KeyframeTranslation() : TimePos(0.0f), Translation(0.0f, 0.0f, 0.0f) {}

KeyframeTranslation::~KeyframeTranslation() {}

KeyframeScale::KeyframeScale() : TimePos(0.0f), Scale(1.0f, 1.0f, 1.0f) {}

KeyframeScale::~KeyframeScale() {}

KeyframeRotation::KeyframeRotation() : TimePos(0.0f), RotationQuat(0.0f, 0.0f, 0.0f, 1.0f) {}

KeyframeRotation::~KeyframeRotation() {}

float BoneAnimation::GetStartTime() const {
	float t1 = TranslationKeyframes.front().TimePos;
	float t2 = ScaleKeyframes.front().TimePos;
	float t3 = RotationKeyframes.front().TimePos;
	return std::max(std::max(t1, t2), t3);
}

float BoneAnimation::GetEndTime() const {
	float t1 = TranslationKeyframes.back().TimePos;
	float t2 = ScaleKeyframes.back().TimePos;
	float t3 = RotationKeyframes.back().TimePos;
	return std::max(std::max(t1, t2), t3);
}

void BoneAnimation::Interpolate(float t, DirectX::XMFLOAT4X4& M) const {
	using namespace DirectX;
	
	size_t sz1 = TranslationKeyframes.size();
	size_t sz2 = ScaleKeyframes.size();
	size_t sz3 = RotationKeyframes.size();
	if ((!sz1) || (!sz2) || (!sz3)) {
		DirectX::XMStoreFloat4x4(&M, DirectX::XMMatrixIdentity());
		return;
	}

	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR identity = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR S = identity;
	XMVECTOR P = zero;
	XMVECTOR Q = zero;

	if (TranslationKeyframes.size() == 1) {
		auto begin_it = TranslationKeyframes.begin();
		P = XMLoadFloat3(&begin_it->Translation);
	}
	else if (t >= TranslationKeyframes.rbegin()->TimePos) {
		auto last_it = TranslationKeyframes.rbegin();
		P = XMLoadFloat3(&last_it->Translation);
	}
	else {
		auto it1 = std::lower_bound(TranslationKeyframes.cbegin(), TranslationKeyframes.cend(), t, [](const auto it, float t) { return it.TimePos < t; });
		auto it0 = std::prev(it1);

		float current_time_pos = it0->TimePos;
		float next_time_pos = it1->TimePos;
		float lerp_percent = (t - current_time_pos) / (next_time_pos - current_time_pos);

		XMVECTOR p0 = XMLoadFloat3(&it0->Translation);
		XMVECTOR p1 = XMLoadFloat3(&it1->Translation);

		P = XMVectorLerp(p0, p1, lerp_percent);
	}

	if (ScaleKeyframes.size() == 1) {
		auto begin_it = ScaleKeyframes.begin();
		S = XMLoadFloat3(&begin_it->Scale);
	}
	else if (t >= ScaleKeyframes.rbegin()->TimePos) {
		auto last_it = ScaleKeyframes.rbegin();
		S = XMLoadFloat3(&last_it->Scale);
	}
	else {
		auto it1 = std::lower_bound(ScaleKeyframes.cbegin(), ScaleKeyframes.cend(), t, [](const auto it, float t) { return it.TimePos < t; });
		auto it0 = std::prev(it1);

		float current_time_pos = it0->TimePos;
		float next_time_pos = it1->TimePos;
		float lerp_percent = (t - current_time_pos) / (next_time_pos - current_time_pos);

		XMVECTOR s0 = XMLoadFloat3(&it0->Scale);
		XMVECTOR s1 = XMLoadFloat3(&it1->Scale);

		S = XMVectorLerp(s0, s1, lerp_percent);
	}

	if (RotationKeyframes.size() == 1) {
		auto begin_it = RotationKeyframes.begin();
		Q = XMLoadFloat4(&begin_it->RotationQuat);
	}
	else if (t >= RotationKeyframes.rbegin()->TimePos) {
		auto last_it = RotationKeyframes.rbegin();
		Q = XMLoadFloat4(&last_it->RotationQuat);
	}
	else {
		auto it1 = std::lower_bound(RotationKeyframes.cbegin(), RotationKeyframes.cend(), t, [](const auto it, float t) { return it.TimePos < t; });
		auto it0 = std::prev(it1);

		float current_time_pos = it0->TimePos;
		float next_time_pos = it1->TimePos;
		float lerp_percent = (t - current_time_pos) / (next_time_pos - current_time_pos);

		XMVECTOR q0 = XMLoadFloat4(&it0->RotationQuat);
		XMVECTOR q1 = XMLoadFloat4(&it1->RotationQuat);

		Q = XMQuaternionSlerp(q0, q1, lerp_percent);
	}
	XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
}

float AnimationClip::GetClipStartTime() const {
	float t = std::numeric_limits<float>::infinity();
	for (uint32_t i = 0; i < BoneAnimations.size(); ++i) {
		t = std::min(t, BoneAnimations[i]->GetStartTime());
	}

	return t;
}

float AnimationClip::GetClipEndTime() const {
	float t = 0.0f;
	for (uint32_t i = 0; i < BoneAnimations.size(); ++i) {
		t = std::max(t, BoneAnimations[i]->GetEndTime());
	}

	return t;
}

void AnimationClip::Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& bone_transforms) const {
	for (uint32_t i = 0; i < BoneAnimations.size(); ++i) {
		if (!BoneAnimations[i]) {
			DirectX::XMStoreFloat4x4(&bone_transforms[i], DirectX::XMMatrixIdentity());
		}
		else {
			BoneAnimations[i]->Interpolate(t, bone_transforms[i]);
		}
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

void SkinnedData::SetBoneHierarchy(std::vector<int> bone_hierarchy) {
	m_bone_hierarchy = std::move(bone_hierarchy);
}

const std::vector<int>& SkinnedData::GetBoneHierarchy() const {
	return m_bone_hierarchy;
}

void SkinnedData::SetBoneOffsets(std::vector<DirectX::XMFLOAT4X4> bone_offsets) {
	m_bone_offsets = std::move(bone_offsets);
}

const std::vector<DirectX::XMFLOAT4X4>& SkinnedData::GetBoneOffsets() const {
	return m_bone_offsets;
}

void SkinnedData::SetBoneOffsetsMap(std::unordered_map<std::string, BoneInfo> bone_offsets_map) {
	m_bone_info_map = std::move(bone_offsets_map);
}

const std::unordered_map<std::string, BoneInfo>& SkinnedData::GetBoneOffsetsMap() const {
	return m_bone_info_map;
}

int SkinnedData::CountBoneOffsetsMap(std::string name) {
	return m_bone_info_map.count(name);
}

void SkinnedData::AddBoneOffsetsInfo(std::string name, BoneInfo bone_info) {
	m_bone_info_map[name] = std::move(bone_info);
}

void SkinnedData::SetAnimations(std::unordered_map<std::string, AnimationClip> animations) {
	m_animations = std::move(animations);
}

void SkinnedData::AddAnimations(std::string name, AnimationClip animations) {
	m_animations[name] = std::move(animations);
}

const std::unordered_map<std::string, AnimationClip>& SkinnedData::GetAnimations() const {
	return m_animations;
}

void SkinnedData::GetFinalTransforms(const std::string& clip_name, float time_pos, std::vector<DirectX::XMFLOAT4X4>& final_transforms) const {
	using namespace DirectX;
	uint32_t num_bones = m_bone_offsets.size();

	std::vector<XMFLOAT4X4> to_parent_transforms(num_bones);
	final_transforms.reserve(num_bones);

	// Interpolate all the bones of this clip at the given time instance.
	auto clip = m_animations.find(clip_name);
	clip->second.Interpolate(time_pos, to_parent_transforms);

	// Traverse the hierarchy and transform all the bones to the root space.
	DirectX::XMFLOAT4X4 def_identity_matrix = {};
	DirectX::XMStoreFloat4x4(&def_identity_matrix, DirectX::XMMatrixIdentity());
	std::vector<XMFLOAT4X4> to_root_transforms(num_bones, def_identity_matrix);

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

bool operator<(const KeyframeTranslation& kf1, const KeyframeTranslation& kf2) {
	return kf1.TimePos < kf2.TimePos;
}

bool operator<(const KeyframeScale& kf1, const KeyframeScale& kf2) {
	return kf1.TimePos < kf2.TimePos;
}

bool operator<(const KeyframeRotation& kf1, const KeyframeRotation& kf2) {
	return kf1.TimePos < kf2.TimePos;
}