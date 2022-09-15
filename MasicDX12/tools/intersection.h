#pragma once

#include <vector>

#include <Windows.h>
#include <DirectXMath.h>

#include "../actors/actor.h"
#include "../tools/math_utitity.h"

class Intersection;
typedef std::vector<Intersection> IntersectionArray;

template <class T>
void InitIntersection(Intersection& intersection, DWORD faceIndex, FLOAT dist, FLOAT u, FLOAT v, ActorId actorId, DWORD* pIndices, T* pVertices, DirectX::FXMMATRIX matWorld);

class Intersection {
public:
	float m_fDist; // distance from ray origin to intersection
	DWORD m_dwFace; // the face index of the intersection
	float m_fBary1; // Barycentric coordinates of the intersection
	float m_fBary2; // Barycentric coordinates of the intersection
	float m_tu; // texture coords of intersection
	float m_tv; // texture coords of intersection
	ActorId m_actorId; // Which actor was intersected if there was one
	DirectX::XMFLOAT3 m_worldLoc; // world location of intersection
	DirectX::XMFLOAT3 m_actorLoc; // actor local coordinates of intersection
	DirectX::XMFLOAT3 m_normal; // normal of intersection

	bool const operator <(Intersection const& other) { return m_fDist < other.m_fDist; }
};

template<class T>
inline void InitIntersection(Intersection& intersection, DWORD faceIndex, FLOAT dist, FLOAT u, FLOAT v, ActorId actorId, DWORD* pIndices, T* pVertices, DirectX::FXMMATRIX matWorld) {
	using namespace DirectX;
	intersection.m_dwFace = faceIndex;
	intersection.m_fDist = dist;
	intersection.m_fBary1 = u;
	intersection.m_fBary2 = v;

	T* v0 = &pVertices[pIndices[3 * faceIndex + 0]];
	T* v1 = &pVertices[pIndices[3 * faceIndex + 1]];
	T* v2 = &pVertices[pIndices[3 * faceIndex + 2]];

	// If all you want is the vertices hit, then you are done. In this sample, we
	// want to show how to infer texture coordinates as well, using the BaryCentric
	// coordinates supplied by D3DXIntersect
	float dtu1 = v1->uv.x - v0->uv.x;
	float dtu2 = v2->uv.x - v0->uv.x;
	float dtv1 = v1->uv.y - v0->uv.y;
	float dtv2 = v2->uv.y - v0->uv.y;
	intersection.m_tu = v0->uv.x + intersection.m_fBary1 * dtu1 + intersection.m_fBary2 * dtu2;
	intersection.m_tv = v0->uv.y + intersection.m_fBary1 * dtv1 + intersection.m_fBary2 * dtv2;

	DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&v0->pos) - DirectX::XMLoadFloat3(&v1->pos);
	DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&v2->pos) - DirectX::XMLoadFloat3(&v1->pos);

	DirectX::XMVECTOR cross = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(a, b));

	DirectX::XMVECTOR actorLoc = BarycentricToVec3(v0->pos, v1->pos, v2->pos, intersection.m_fBary1, intersection.m_fBary2);
	DirectX::XMStoreFloat3(&intersection.m_actorLoc, actorLoc);
	DirectX::XMStoreFloat3(&intersection.m_worldLoc, DirectX::XMVector4Transform(actorLoc, matWorld));
	intersection.m_actorId = actorId;
	DirectX::XMStoreFloat3(&intersection.m_normal, cross);
}