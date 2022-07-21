#include "ray_cast.h"

RayCast::RayCast(Point point, DWORD maxIntersections) {
	m_MaxIntersections = maxIntersections;
	m_IntersectionArray.reserve(m_MaxIntersections);
	m_bUseD3DXIntersect = true;
	m_bAllHits = true;
	m_NumIntersections = 0;
	m_Point = point;
}

void RayCast::Sort() {
	std::sort(m_IntersectionArray.begin(), m_IntersectionArray.end());
}