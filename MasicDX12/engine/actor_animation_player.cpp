#include "actor_animation_player.h"

#include "../actors/transform_component.h"
#include "../events/evt_data_new_actor.h"
#include "../events/i_event_manager.h"
#include "engine.h"
#include "base_engine_logic.h"
#include "../tools/string_utility.h"

ActorAnimationPlayer::KeyframeTranslation::KeyframeTranslation() : TimePos(0.0f), Translation(0.0f, 0.0f, 0.0f) {}

ActorAnimationPlayer::KeyframeTranslation::~KeyframeTranslation() {}

ActorAnimationPlayer::KeyframeScale::KeyframeScale() : TimePos(0.0f), Scale(1.0f, 1.0f, 1.0f) {}

ActorAnimationPlayer::KeyframeScale::~KeyframeScale() {}

ActorAnimationPlayer::KeyframeRotation::KeyframeRotation() : TimePos(0.0f), RotationQuat(0.0f, 0.0f, 0.0f, 1.0f) {}

ActorAnimationPlayer::KeyframeRotation::~KeyframeRotation() {}

bool operator<(const ActorAnimationPlayer::KeyframeTranslation& kf1, const ActorAnimationPlayer::KeyframeTranslation& kf2) {
	return kf1.TimePos < kf2.TimePos;
}

bool operator<(const ActorAnimationPlayer::KeyframeScale& kf1, const ActorAnimationPlayer::KeyframeScale& kf2) {
	return kf1.TimePos < kf2.TimePos;
}

bool operator<(const ActorAnimationPlayer::KeyframeRotation& kf1, const ActorAnimationPlayer::KeyframeRotation& kf2) {
	return kf1.TimePos < kf2.TimePos;
}

ActorAnimationPlayer::ActorAnimationPlayer() {
	m_time.ResetDuration();
	m_animation_state = AnimState::Playing;
	m_total_animation_time = 1.0f;
}

ActorAnimationPlayer::~ActorAnimationPlayer() {}

bool ActorAnimationPlayer::Initialize(const pugi::xml_node& pLevel_data) {
	RegisterAllDelegates();

	bool result = true;

	pugi::xml_node animations_node = pLevel_data.child("Scene").child("ActorAnimations");
	if (!animations_node) { return result; }

	m_total_animation_time = animations_node.attribute("TotalTimeSec").as_float();
	
	for (pugi::xml_node node = animations_node.first_child(); node; node = node.next_sibling()) {
		AddActorAnimation(node);
	}
	
	return result;
}

void ActorAnimationPlayer::Update(const GameTimerDelta& delta) {
	//if (m_animation_state == AnimState::Stoped) return;

	if (m_animation_state == AnimState::Playing) {
		m_time.AddDeltaDuration(delta);
	}
	float t = m_time.fGetTotalSeconds();
	for (auto&[actor_ptr, anim] : m_actors_animation_map) {
		DirectX::XMFLOAT4X4 transform;
		anim.Interpolate(t, transform);
		DirectX::XMMATRIX transform_xm = DirectX::XMLoadFloat4x4(&transform);
		std::shared_ptr<TransformComponent> tc = actor_ptr->GetComponent<TransformComponent>().lock();
		if (tc) {
			tc->SetTransform(transform);
		}
	}
}

void ActorAnimationPlayer::Pause() {
	m_animation_state = AnimState::Stoped;
}

void ActorAnimationPlayer::Stop() {
	m_animation_state = AnimState::Stoped;
	m_time.ResetDuration();
}

void ActorAnimationPlayer::Play() {
	m_animation_state = AnimState::Playing;
}

void ActorAnimationPlayer::SetDuration(float t) {
	m_time.ResetDuration();
	m_time.AddDeltaDuration(t);
}

void ActorAnimationPlayer::SetDuration(const GameTimerDelta& duration) {
	m_time.ResetDuration();
	m_time.AddDeltaDuration(duration);
}

float ActorAnimationPlayer::GetTotalAnimationTime() {
	return m_total_animation_time;
}

float ActorAnimationPlayer::GetCurrentAnimationTime() {
	return m_time.fGetTotalSeconds();
}

ActorAnimationPlayer::AnimMap& ActorAnimationPlayer::GetAnimMap() {
	return m_actors_animation_map;
}

void ActorAnimationPlayer::RegisterAllDelegates() {
	IEventManager* pGlobalEventManager = IEventManager::Get();
}

void ActorAnimationPlayer::AddActorAnimation(const pugi::xml_node& pAnim_data) {
	pugi::xml_node actor_name_node = pAnim_data.child("AnimationForActorName");
	if (!actor_name_node) return;
	std::string actor_name = actor_name_node.text().as_string();

	std::shared_ptr<Engine> engine = Engine::GetEngine();
	std::shared_ptr<BaseEngineLogic> engine_logic = engine->GetGameLogic();
	std::shared_ptr<Actor> actor_ptr = engine_logic->VGetActorByName(actor_name).lock();
	if (!actor_ptr) return;
	std::shared_ptr<TransformComponent> tc = actor_ptr->GetComponent<TransformComponent>().lock();
	if (!tc) return;

	if (!m_actors_animation_map.count(actor_ptr)) {
		DirectX::XMMATRIX transform_xm = tc->GetTransform();
		DirectX::XMVECTOR scale_xm;
		DirectX::XMVECTOR rotation_xm;
		DirectX::XMVECTOR translation_xm;
		DirectX::XMMatrixDecompose(&scale_xm, &rotation_xm, &translation_xm, transform_xm);

		ActorAnimation act_anim;

		KeyframeTranslation first_translation_frame;
		first_translation_frame.TimePos = 0.0f;
		DirectX::XMStoreFloat3(&first_translation_frame.Translation, translation_xm);
		act_anim.TranslationKeyframes.push_back(first_translation_frame);

		KeyframeScale first_scale_frame;
		first_scale_frame.TimePos = 0.0f;
		DirectX::XMStoreFloat3(&first_scale_frame.Scale, scale_xm);
		act_anim.ScaleKeyframes.push_back(first_scale_frame);

		KeyframeRotation first_rotation_frame;
		first_rotation_frame.TimePos = 0.0f;
		DirectX::XMStoreFloat4(&first_rotation_frame.RotationQuat, rotation_xm);
		act_anim.RotationKeyframes.push_back(first_rotation_frame);

		m_actors_animation_map[actor_ptr] = act_anim;
	}
	ActorAnimation& act_anim = m_actors_animation_map[actor_ptr];

	pugi::xml_node kf_node = pAnim_data.child("RelativeKeyframe");
	if (!kf_node) return;

	pugi::xml_node time_node = kf_node.child("TimePosSec");
	float time_pos = time_node.text().as_float();

	pugi::xml_node trans_node = kf_node.child("Translation");
	if (trans_node) {
		KeyframeTranslation trans_frame;
		trans_frame.TimePos = time_pos;
		DirectX::XMFLOAT3 pos = posfromattr3f(trans_node);
		trans_frame.Translation = pos;
		act_anim.TranslationKeyframes.push_back(trans_frame);
	}

	pugi::xml_node ypr_node = kf_node.child("YawPitchRoll");
	if (ypr_node) {
		KeyframeRotation rotation_frame;
		rotation_frame.TimePos = time_pos;
		DirectX::XMFLOAT3 yaw_pitch_roll = anglesfromattr3f(ypr_node);
		//DirectX::XMQuaternionRotationRollPitchYaw(yaw_pitch_roll.z, yaw_pitch_roll.y, yaw_pitch_roll.x);
		DirectX::XMStoreFloat4(&rotation_frame.RotationQuat, DirectX::XMQuaternionRotationRollPitchYaw(yaw_pitch_roll.y, yaw_pitch_roll.x, yaw_pitch_roll.z));
		act_anim.RotationKeyframes.push_back(rotation_frame);
	}

	pugi::xml_node scale_node = kf_node.child("Scale");
	if (scale_node) {
		KeyframeScale scale_frame;
		scale_frame.TimePos = time_pos;
		DirectX::XMFLOAT3 scale = posfromattr3f(scale_node);
		scale_frame.Scale = scale;
		act_anim.ScaleKeyframes.push_back(scale_frame);
	}
}

void ActorAnimationPlayer::ActorAnimation::Interpolate(float t, DirectX::XMFLOAT4X4& transform) const {
	using namespace DirectX;

	size_t sz1 = TranslationKeyframes.size();
	size_t sz2 = ScaleKeyframes.size();
	size_t sz3 = RotationKeyframes.size();
	if ((!sz1) || (!sz2) || (!sz3)) {
		DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());
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
		if (it1 == TranslationKeyframes.cbegin()) {
			P = XMLoadFloat3(&it1->Translation);
		}
		else {
			auto it0 = std::prev(it1);

			float current_time_pos = it0->TimePos;
			float next_time_pos = it1->TimePos;
			float time_delta = next_time_pos - current_time_pos;
			float lerp_percent = 0.5f;
			if (time_delta > 0.0001f) {
				lerp_percent = (t - current_time_pos) / (next_time_pos - current_time_pos);
			}

			XMVECTOR p0 = XMLoadFloat3(&it0->Translation);
			XMVECTOR p1 = XMLoadFloat3(&it1->Translation);

			P = XMVectorLerp(p0, p1, lerp_percent);
		}
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
		if (it1 == ScaleKeyframes.cbegin()) {
			S = XMLoadFloat3(&it1->Scale);
		}
		else {
			auto it0 = std::prev(it1);

			float current_time_pos = it0->TimePos;
			float next_time_pos = it1->TimePos;
			float time_delta = next_time_pos - current_time_pos;
			float lerp_percent = 0.5f;
			if (time_delta > 0.0001f) {
				lerp_percent = (t - current_time_pos) / (next_time_pos - current_time_pos);
			}

			XMVECTOR s0 = XMLoadFloat3(&it0->Scale);
			XMVECTOR s1 = XMLoadFloat3(&it1->Scale);

			S = XMVectorLerp(s0, s1, lerp_percent);
		}
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
		if (it1 == RotationKeyframes.cbegin()) {
			Q = XMLoadFloat4(&it1->RotationQuat);
		}
		else {
			auto it0 = std::prev(it1);

			float current_time_pos = it0->TimePos;
			float next_time_pos = it1->TimePos;
			float time_delta = next_time_pos - current_time_pos;
			float lerp_percent = 0.5f;
			if (time_delta > 0.0001f) {
				lerp_percent = (t - current_time_pos) / (next_time_pos - current_time_pos);
			}

			XMVECTOR q0 = XMLoadFloat4(&it0->RotationQuat);
			XMVECTOR q1 = XMLoadFloat4(&it1->RotationQuat);

			Q = XMQuaternionSlerp(q0, q1, lerp_percent);
		}
	}
	XMStoreFloat4x4(&transform, XMMatrixAffineTransformation(S, zero, Q, P));
}