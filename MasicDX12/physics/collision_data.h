#pragma once

#include "contact.h"

struct CollisionData {
    Contact* contactArray;
    Contact* contacts;
    int contactsLeft;
    unsigned contactCount;
    float friction;
    float restitution;
    float tolerance;

    bool hasMoreContacts();
    void reset(unsigned maxContacts);
    void addContacts(unsigned count);
};