#include "force_registry.h"

void ForceRegistry::add(RigidBody* body, ForceGenerator* fg) {
    ForceRegistration registration;
    registration.body = body;
    registration.fg = fg;
    registrations.push_back(registration);
}

void ForceRegistry::remove(RigidBody* body, ForceGenerator* fg) {}

void ForceRegistry::clear() {}

void ForceRegistry::updateForces(float duration) {
    Registry::iterator i = registrations.begin();
    for (; i != registrations.end(); i++) {
        i->fg->updateForce(i->body, duration);
    }
}