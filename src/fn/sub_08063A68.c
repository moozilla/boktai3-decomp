// MATCHING_CONCESSION: register-pinning
#include "roaming_object.h"
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
void RoamingObject_ApplyImpulse(struct RoamingObject *owner)
{
    owner->animation = 2;
    if (owner->impulse) {
        s32 x = owner->impulse;
        register const s16 *table asm("r2") = gUnk_086149C4;
        x = Scale(table[(owner->angle + 64) & 255] * x);

        owner->velocity.x += x;
        {
            register s32 y asm("r1") = owner->impulse;
            y = Scale(table[owner->angle] * y);
            asm("" : "+r"(y));
            owner->velocity.z += y;
        }
        owner->impulse = (owner->impulse * 3) >> 2;
    } else {
        owner->angle += 128;
        owner->targetAngle = owner->angle;
        {
            void *listener = &owner->listener;
            s32 mask = -2;
            ((typeof(owner->listener) *)listener)->flags &= mask;
        }
        RoamingObject_SetState(owner, 0);
    }
}
