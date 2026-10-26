#include "roaming_object.h"
void RoamingObject_SteerWander(struct RoamingObject *owner)
{
    u32 index_2;
    if (owner->angle != owner->targetAngle) {
        if ((((owner->targetAngle - owner->angle) + 256) & 255) <= 127) {
            owner->angle += 4;
        } else {
            owner->angle += 252;
        }
    }
    if (owner->turnTimer == 0) {
        const u16 *table = gUnk_0203B400;
        u32 *counter = &gRandomIndex;
        u32 index;
        u32 mask;
        u32 sample;
        index = *counter + 1;
        mask = 1023;
        index &= mask;
        *(&sample) = table[index];
        sample &= 3;
        sample <<= 6;
        owner->targetAngle = sample;
        index_2 = (index + 1) & mask;
        *counter = index_2;
        sample = table[index_2];
        sample &= 63;
        sample += 120;
        owner->turnTimer = sample;
    } else {
        owner->turnTimer--;
    }
    Collider_SetPosition(&owner->collider, &owner->position, 0);
    Collider_Refresh(&owner->collider);
    owner->animation = 7;
}
