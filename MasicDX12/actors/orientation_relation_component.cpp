#include "orientation_relation_component.h"
#include "../engine/engine.h"
#include "../tools/memory_utility.h"
#include "../tools/math_utitity.h"
#include "../tools/string_utility.h"
#include "particle_component.h"
#include "transform_component.h"
#include "enemy_component.h"

const std::string OrientationRelationComponent::g_Name = "OrientationRelationComponent"s;

const std::string& OrientationRelationComponent::VGetName() const {
	return OrientationRelationComponent::g_Name;
}

OrientationRelationComponent::OrientationRelationComponent() {
	m_relate_to = "NoRelation";
	m_first = true;
	m_at_speed = 0.1f;
}

OrientationRelationComponent::~OrientationRelationComponent() {}

pugi::xml_node OrientationRelationComponent::VGenerateXml() {
	return pugi::xml_node();
}

void OrientationRelationComponent::VRegisterEvents() {}

bool OrientationRelationComponent::VInit(const pugi::xml_node& data) {

	pugi::xml_node relate_node = data.child("RelateTo");
	if (relate_node) {
		m_relate_to = relate_node.value();
	}

	m_at_speed = data.child("AtSpeed").text().as_float(m_at_speed);

	return true;
}

void OrientationRelationComponent::VPostInit() {}

void OrientationRelationComponent::VUpdate(const GameTimerDelta& delta) {
	using namespace DirectX;
	float deltaMs = delta.fGetDeltaMilliseconds();
	std::shared_ptr<Actor> act = GetOwner();
	std::shared_ptr<TransformComponent> tc = MakeStrongPtr(act->GetComponent<TransformComponent>(ActorComponent::GetIdFromName("TransformComponent")));
	if (m_first) {
		MTRandom& rnd = Engine::GetEngine()->GetGameLogic()->GetRNG();

		std::shared_ptr<Actor> related_act = MakeStrongPtr(VGetRelatedToActor());
		std::shared_ptr<TransformComponent> related_tc = MakeStrongPtr(related_act->GetComponent<TransformComponent>(ActorComponent::GetIdFromName("TransformComponent")));

		XMVECTOR pos = tc->GetPosition();
		XMVECTOR center = related_tc->GetPosition();
		XMVECTOR direction = pos - center;
		XMVectorSetW(direction, 0.0f);
		XMVECTOR normal = XMVector3Normalize(direction);

		XMVECTOR up = normal;
		float x = rnd.Random() * 2.0f - 1.0f; float y = rnd.Random() * 2.0f - 1.0f; float z = rnd.Random() * 2.0f - 1.0f;
		XMVECTOR at = XMVector3Normalize(XMVectorSet(x, y, z, 0.0f));
		XMVECTOR right = XMVector3Cross(up, at);
		at = XMVector3Cross(right, up);

		XMStoreFloat4(&m_at, at);
		XMStoreFloat4(&m_up, up);
		XMStoreFloat4(&m_right, right);

		XMFLOAT3 sc = tc->GetScale3f();
		XMFLOAT3 p = tc->GetPosition3f();

		XMMATRIX rot{
			m_right.x, m_right.y, m_right.z, 0.0f,
			m_up.x,    m_up.y,    m_up.z,    0.0f,
			m_at.x,    m_at.y,    m_at.z,    0.0f,
			0.0f,      0.0f,      0.0f,      1.0f
		};

		XMFLOAT4X4 transform;
		DirectX::XMStoreFloat4x4(
			&transform,
			DirectX::XMMatrixMultiply(
				DirectX::XMMatrixMultiply(
					rot,
					DirectX::XMMatrixScaling(sc.x, sc.y, sc.z)
				),
				XMMatrixTranslation(p.x, p.y, p.z)
			)
		);
		tc->SetTransform(transform);

		m_first = false;
	}
	else {
		std::shared_ptr<OrientationRelationComponent> oc = MakeStrongPtr(act->GetComponent<OrientationRelationComponent>(ActorComponent::GetIdFromName("OrientationRelationComponent")));
		std::shared_ptr<Actor> related_act = MakeStrongPtr(oc->VGetRelatedToActor());
		std::shared_ptr<TransformComponent> related_tc = MakeStrongPtr(related_act->GetComponent<TransformComponent>(ActorComponent::GetIdFromName("TransformComponent")));
		std::shared_ptr<ParticleComponent> pc = MakeStrongPtr(act->GetComponent<ParticleComponent>(ActorComponent::GetIdFromName("ParticleComponent")));
		std::shared_ptr<EnemyComponent> ec = MakeStrongPtr(act->GetComponent<EnemyComponent>(ActorComponent::GetIdFromName("EnemyComponent")));

		XMVECTOR pos = tc->GetPosition();
		XMVECTOR center = related_tc->GetPosition();
		XMVECTOR direction = pos - center;
		XMVectorSetW(direction, 0.0f);
		XMVECTOR normal = XMVector3Normalize(direction);

		XMVECTOR up = normal;
		XMVECTOR at = XMVector3Cross(XMVector3Normalize(XMLoadFloat4(&m_right)), up);;
		XMVECTOR right = XMVector3Cross(up, at);
		if (pc && ec) {
			const Particle& particle = pc->VGetParticle();
			XMVECTOR velocity = particle.getVelocity() * deltaMs;
			float at_proj = XMVectorGetX(XMVector3Dot(at, velocity));
			float right_proj = XMVectorGetX(XMVector3Dot(right, velocity));
			float up_proj = XMVectorGetX(XMVector3Dot(up, velocity));
			XMVECTOR ground_move = at * at_proj + right * right_proj;
			float ground_move_len = XMVectorGetX(XMVector3Length(ground_move));
			if (particle.getAwake() && ground_move_len > 0.0005f) {
				XMVECTOR velocity_normal = XMVector3Normalize(ground_move);
				right = XMVector3Cross(velocity_normal, up);;
				at = XMVector3Cross(right, up);
			}

		}

		XMStoreFloat4(&m_at, at);
		XMStoreFloat4(&m_up, up);
		XMStoreFloat4(&m_right, right);

		XMFLOAT3 sc = tc->GetScale3f();
		XMFLOAT3 p = tc->GetPosition3f();

		XMMATRIX rot{
			m_right.x, m_right.y, m_right.z, 0.0f,
			m_up.x,    m_up.y,    m_up.z,    0.0f,
			m_at.x,    m_at.y,    m_at.z,    0.0f,
			0.0f,      0.0f,      0.0f,      1.0f
		};

		XMFLOAT4X4 transform;
		DirectX::XMStoreFloat4x4(
			&transform,
			DirectX::XMMatrixMultiply(
				DirectX::XMMatrixMultiply(
					rot,
					DirectX::XMMatrixScaling(sc.x, sc.y, sc.z)
				),
				XMMatrixTranslation(p.x, p.y, p.z)
			)
		);

		tc->SetTransform(transform);
	}
}

const std::string& OrientationRelationComponent::VGetRelatedToName() {
	return m_relate_to;
}

WeakActorPtr OrientationRelationComponent::VGetRelatedToActor() {
	return Engine::GetEngine()->GetGameLogic()->VGetActorByName(m_relate_to);
}

const DirectX::XMFLOAT4& OrientationRelationComponent::VGetAt4f() {
	return m_at;
}

const DirectX::XMFLOAT4& OrientationRelationComponent::VGetUp4f() {
	return m_up;
}

const DirectX::XMFLOAT4& OrientationRelationComponent::VGetRight4f() {
	return m_right;
}

DirectX::XMVECTOR OrientationRelationComponent::VGetAt() {
	return DirectX::XMLoadFloat4(&m_at);
}

DirectX::XMVECTOR OrientationRelationComponent::VGetUp() {
	return DirectX::XMLoadFloat4(&m_up);
}

DirectX::XMVECTOR OrientationRelationComponent::VGetRight() {
	return DirectX::XMLoadFloat4(&m_right);
}

DirectX::XMMATRIX OrientationRelationComponent::VGetOrient() {
	return DirectX::XMMATRIX{
			m_right.x, m_right.y, m_right.z, 0.0f,
			m_up.x,    m_up.y,    m_up.z,    0.0f,
			m_at.x,    m_at.y,    m_at.z,    0.0f,
			0.0f,      0.0f,      0.0f,      1.0f
	};
}

DirectX::XMFLOAT4X4 OrientationRelationComponent::VGetOrient4x4f() {
	return DirectX::XMFLOAT4X4{
			m_right.x, m_right.y, m_right.z, 0.0f,
			m_up.x,    m_up.y,    m_up.z,    0.0f,
			m_at.x,    m_at.y,    m_at.z,    0.0f,
			0.0f,      0.0f,      0.0f,      1.0f
	};
}

void OrientationRelationComponent::VRotateUp(float angle_deg) {
	using namespace DirectX;

	XMStoreFloat4(&m_right, XMVector3Transform(VGetRight(), XMMatrixRotationAxis(VGetUp(), XMConvertToRadians(angle_deg))));
	XMStoreFloat4(&m_at, XMVector3Transform(VGetAt(), XMMatrixRotationAxis(VGetUp(), XMConvertToRadians(angle_deg))));
}
