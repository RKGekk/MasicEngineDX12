#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <DirectXMath.h>

#include "../actors/actor.h"
#include "../events/i_event_data.h"

class ActorAnimationPlayer {
public:
	struct KeyframeTranslation {
		KeyframeTranslation();
		~KeyframeTranslation();

		float TimePos;
		DirectX::XMFLOAT3 Translation;
	};
	friend bool operator<(const KeyframeTranslation& kf1, const KeyframeTranslation& kf2);

	struct KeyframeScale {
		KeyframeScale();
		~KeyframeScale();

		float TimePos;
		DirectX::XMFLOAT3 Scale;
	};
	friend bool operator<(const KeyframeScale& kf1, const KeyframeScale& kf2);

	struct KeyframeRotation {
		KeyframeRotation();
		~KeyframeRotation();

		float TimePos;
		DirectX::XMFLOAT4 RotationQuat;

		friend bool operator<(const KeyframeRotation& kf1, const KeyframeRotation& kf2);
	};
	friend bool operator<(const KeyframeRotation& kf1, const KeyframeRotation& kf2);

	struct ActorAnimation {
		void Interpolate(float t, DirectX::XMFLOAT4X4& transform) const;

		// ASC sorted by time
		std::vector<KeyframeTranslation> TranslationKeyframes;
		std::vector<KeyframeScale> ScaleKeyframes;
		std::vector<KeyframeRotation> RotationKeyframes;
	};

	using AnimMap = std::unordered_map<StrongActorPtr, ActorAnimation>;

	enum class AnimState {
		Playing,
		Stoped
	};

	ActorAnimationPlayer();
	~ActorAnimationPlayer();

	bool Initialize(const pugi::xml_node& pLevel_data);
	void Update(const GameTimerDelta& delta);

	void Pause();
	void Stop();
	void Play();
	void SetDuration(float t);
	void SetDuration(const GameTimerDelta& duration);

	float GetTotalAnimationTime();
	float GetCurrentAnimationTime();
	AnimMap& GetAnimMap();
	
private:
	void RegisterAllDelegates();
	void AddActorAnimation(const pugi::xml_node& pAnim_data);

	AnimMap m_actors_animation_map;
	GameTimerDelta m_time;
	AnimState m_animation_state;
	float m_total_animation_time;
};