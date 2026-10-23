#include "roaming_object.h"
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
void RoamingObjectB_ApplyImpulse(struct RoamingObject *owner)
{
    owner->animation = 2;
    if (owner->impulse) {
        s32 x = owner->impulse;
        const s16 *table = gUnk_086149C4;
        x = Scale(table[(owner->angle + 64) & 255] * x);

        owner->velocity.x += x;
        {
            s32 y = owner->impulse;
            ;
            owner->velocity.z += Scale(y * gUnk_086149C4[owner->angle]);
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
        RoamingObjectB_SetState(owner, 0);
    }
}
