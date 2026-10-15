// MATCHING_CONCESSION: register-pinning
#include "roaming_object.h"
#define RoamingObjectB_SteerWander sub_08063FFC
void RoamingObjectB_SteerWander(struct RoamingObject *owner)
{
    if (owner->angle != owner->targetAngle) {
        if (((owner->targetAngle - owner->angle + 256) & 255) <= 127)
            owner->angle += 4;
        else
            owner->angle += 252;
    }
    if (owner->turnTimer == 0) {
        const u16 *table = gUnk_0203B400;
        u32 *counter = &gUnk_03005308;
        register u32 index asm("r1");
        u32 mask;
        register u32 sample asm("r0");
        index = *counter + 1;
        mask = 1023;
        index &= mask;
        sample = table[index];
        sample &= 3;
        sample <<= 6;
        owner->targetAngle = sample;
        index = (index + 1) & mask;
        *counter = index;
        sample = table[index];
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
