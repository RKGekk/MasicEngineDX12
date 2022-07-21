#pragma once

#include <algorithm>

#include <DirectXMath.h>

#include "point.h"
#include "../graphics/vertex.h"
#include "../graphics/vertex_buffer.h"
#include "../graphics/index_buffer.h"
#include "../tools/intersection.h"

class Scene;

class RayCast {
protected:
	VertexBuffer<Vertex>* m_pVB;

public:
	RayCast(Point point, DWORD maxIntersections = 16);

	DWORD m_MaxIntersections;
	DWORD m_NumIntersections;
	bool m_bUseD3DXIntersect;
	bool m_bAllHits;
	Point m_Point;

	DirectX::XMFLOAT3 m_vPickRayDir;
	DirectX::XMFLOAT3 m_vPickRayOrig;

	IntersectionArray m_IntersectionArray;

	HRESULT Pick(Scene* pScene, ActorId actorId, VertexBuffer<Vertex>* pVerts, IndexBuffer* pIndices, DWORD numPolys);
	HRESULT Pick(Scene* pScene, ActorId actorId, VertexBuffer<Vertex>* pVerts, DWORD numPolys);

	void Sort();
};