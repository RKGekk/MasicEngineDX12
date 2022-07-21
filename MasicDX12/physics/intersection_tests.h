#pragma once

#include "collision_sphere.h"
#include "collision_plane.h"
#include "collision_box.h"

class IntersectionTests {
public:

    static bool sphereAndHalfSpace(const CollisionSphere& sphere, const CollisionPlane& plane);
    static bool sphereAndSphere(const CollisionSphere& one, const CollisionSphere& two);
    static bool boxAndBox(const CollisionBox& one, const CollisionBox& two);
    static bool boxAndHalfSpace(const CollisionBox& box, const CollisionPlane& plane);
};