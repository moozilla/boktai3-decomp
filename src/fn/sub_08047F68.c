#include "contact_burst.h"
struct ContactBurstHit {
    u8 unknown0[6];
    u16 flags;
    u8 unknown8[0x1C];
    s16 x, y, z;
    u8 unknown2A[0x18];
    u8 direction;
};
void ContactBurst_OnContact(struct ContactBurstHit *self, struct ContactBurstHit *otherInput,
                            struct ContactBurst *effectInput)
{
    struct ContactBurstHit *other = otherInput;
    struct ContactBurst *effect = effectInput;
    u32 direction;
    do {
        effect->contactTimer = 18;
    } while (0);
    {
        u32 mask = 256;
        if (self->flags & mask)
            direction = self->direction;
        else
            direction = Math_Atan2(other->x - self->x, other->z - self->z);
    }
    effect->contactDirection = direction;
}
