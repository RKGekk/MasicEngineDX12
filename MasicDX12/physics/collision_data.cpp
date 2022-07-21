#include "collision_data.h"

bool CollisionData::hasMoreContacts() {
	return contactsLeft > 0;
}

void CollisionData::reset(unsigned maxContacts) {
	contactsLeft = maxContacts;
	contactCount = 0;
	contacts = contactArray;
}

void CollisionData::addContacts(unsigned count) {
	contactsLeft -= count;
	contactCount += count;
	contacts += count;
}
