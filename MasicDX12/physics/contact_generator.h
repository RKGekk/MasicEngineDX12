#pragma once

#include "contact.h"

class ContactGenerator {
public:
    virtual unsigned addContact(Contact* contact, unsigned limit) const = 0;
};