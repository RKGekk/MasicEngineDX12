#include "intersection_tests.h"

#include <cmath>

bool IntersectionTests::sphereAndHalfSpace(const CollisionSphere& sphere, const CollisionPlane& plane) {
    using namespace DirectX;

    float ballDistance = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&plane.direction), sphere.getAxis(3))) - sphere.radius;
    return ballDistance <= plane.offset;
}

bool IntersectionTests::sphereAndSphere(const CollisionSphere& one, const CollisionSphere& two) {
    using namespace DirectX;

    XMVECTOR midline = one.getAxis(3) - two.getAxis(3);
    return XMVectorGetX(XMVector3LengthSq(midline)) < ((one.radius + two.radius) * (one.radius + two.radius));
}

static inline float transformToAxis(const CollisionBox& box, DirectX::FXMVECTOR axis) {
    using namespace DirectX;

    return (
        box.halfSize.x * std::fabsf(XMVectorGetX(XMVector3Dot(axis, box.getAxis(0)))) +
        box.halfSize.y * std::fabsf(XMVectorGetX(XMVector3Dot(axis, box.getAxis(1)))) +
        box.halfSize.z * std::fabsf(XMVectorGetX(XMVector3Dot(axis, box.getAxis(2))))
    );
}

static inline bool overlapOnAxis(const CollisionBox& one, const CollisionBox& two, DirectX::FXMVECTOR axis, DirectX::FXMVECTOR toCentre) {
    using namespace DirectX;
    
    float oneProject = transformToAxis(one, axis);
    float twoProject = transformToAxis(two, axis);
    float distance = std::fabsf(XMVectorGetX(XMVector3Dot(toCentre, axis)));

    return (distance < oneProject + twoProject);
}

bool IntersectionTests::boxAndBox(const CollisionBox& one, const CollisionBox& two) {
    using namespace DirectX;
    
    XMVECTOR toCentre = two.getAxis(3) - one.getAxis(3);

    return (
        overlapOnAxis(one, two, one.getAxis(0), toCentre) &&
        overlapOnAxis(one, two, one.getAxis(1), toCentre) &&
        overlapOnAxis(one, two, one.getAxis(2), toCentre) &&

        overlapOnAxis(one, two, two.getAxis(0), toCentre) &&
        overlapOnAxis(one, two, two.getAxis(1), toCentre) &&
        overlapOnAxis(one, two, two.getAxis(2), toCentre) &&

        overlapOnAxis(one, two, XMVector3Cross(one.getAxis(0), two.getAxis(0)), toCentre) &&
        overlapOnAxis(one, two, XMVector3Cross(one.getAxis(0), two.getAxis(1)), toCentre) &&
        overlapOnAxis(one, two, XMVector3Cross(one.getAxis(0), two.getAxis(2)), toCentre) &&
        overlapOnAxis(one, two, XMVector3Cross(one.getAxis(1), two.getAxis(0)), toCentre) &&
        overlapOnAxis(one, two, XMVector3Cross(one.getAxis(1), two.getAxis(1)), toCentre) &&
        overlapOnAxis(one, two, XMVector3Cross(one.getAxis(1), two.getAxis(2)), toCentre) &&
        overlapOnAxis(one, two, XMVector3Cross(one.getAxis(2), two.getAxis(0)), toCentre) &&
        overlapOnAxis(one, two, XMVector3Cross(one.getAxis(2), two.getAxis(1)), toCentre) &&
        overlapOnAxis(one, two, XMVector3Cross(one.getAxis(2), two.getAxis(2)), toCentre)
    );
}

bool IntersectionTests::boxAndHalfSpace(const CollisionBox& box, const CollisionPlane& plane) {
    using namespace DirectX;

    float projectedRadius = transformToAxis(box, XMLoadFloat3(&plane.direction));
    float boxDistance = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&plane.direction), box.getAxis(3))) - projectedRadius;

    return boxDistance <= plane.offset;
}
