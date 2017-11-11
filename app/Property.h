#ifndef PROPERTY_H
#define PROPERTY_H

typedef enum
{
    PROPERTY_ID_POWER = 0,
    PROPERTY_ID_GEAR,
    PROPERTY_ID_PM2p5,
    PROPERTY_ID_LIGHTNESS,
    PROPERTY_ID_INFRARED,
    PROPERTY_ID_CONSUMABLES,
    PROPERTY_ID_COUNT,
}PropertyID_t;

uint16_t PropertyGetValue(PropertyID_t id);
void PropertySetValue(PropertyID_t id, uint16_t value);
void PropertyInitialize(void);
void PorpertyPoll(void);

#endif

