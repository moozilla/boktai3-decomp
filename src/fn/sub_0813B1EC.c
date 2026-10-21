#include "global.h"
#include "player.h"
#include "contact_actor.h"

static inline u32 flagBits(u32 *flags, u32 mask)
{
    u32 bits = mask;
    return *flags & bits;
}

s32 Math_Atan2(s32, s32);
s32 ActorDirection_ComputeRelativeCardinal(struct ContactCollider *, struct ContactCollider *);

void Player_ApplyContactResponse(struct Player *p, struct ContactCollider *source,
                                 struct ContactCollider *target)
{
    s16 k2 = 0;
    u16 *response;
    u16 *in;
    u16 v;
    u16 *t;
    s32 zero;
    if (p->positionResetState == 1) {
        target->responseParameter = 0;
    }
    in = &source->responseParameter;
    v = *in;
    t = &target->responseParameter;
    zero = k2;
    *t = v;
    v = *in;
    response = t;
    if (v > 200) {
        *response = 200;
    }
    if (flagBits(&source->reactionFlags, 0x40000)) {
        p->contactSpecial = 1;
        goto compute;
    }
    if (p->tetherMode == 5) {
        u32 cur = target->flags;
        target->flags = cur | 4;
        *response = 0;
        return;
    }
    if (flagBits((u32 *)((u8 *)p + 0x20), 0x8000)) {
        s32 dir;
        if (source->flags & 0x100) {
            dir = ((source->direction + 0x70) >> 5) & 7;
        } else {
            s32 dx = source->x - target->x;
            s32 dy = source->z - target->z;
            if (dx == 0 && dy == 0) {
                p->contactDirection = p->facing;
                goto done;
            }
            dir = (((u16)Math_Atan2(dx, dy) + 0x70) >> 5) & 7;
            p->contactDirection = (dir + 4) & 7;
            goto after;
        }
        p->contactDirection = dir;
    after:
    done:
        p->contactMagnitude = *response;
        *response = 0;
        target->flags |= 4;
        return;
    }
    p->contactSpecial = zero;
compute:
    p->facing = ActorDirection_ComputeRelativeCardinal(source, target);
}
