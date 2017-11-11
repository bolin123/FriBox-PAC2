#include "Property.h"

static uint16_t g_properties[PROPERTY_ID_COUNT];

uint16_t PropertyGetValue(PropertyID_t id)
{
    return g_properties[id];
}

void PropertySetValue(PropertyID_t id, uint16_t value)
{
    g_properties[id] = value;
}

void PropertyInitialize(void)
{
}

void PorpertyPoll(void)
{
}

