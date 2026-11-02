#include "global.h"
#include "player.h"
#include "contact_actor.h"

static inline u32 flagBits(u32 *flags, u32 mask)
{
    u32 bits = mask;
    return *flags & bits;
}

s32 Math_Atan2(s32, s32);
s32 ContactCollider_GetImpactDirection(struct ContactCollider *, struct ContactCollider *);

// confidence=low: linked action contact handler mirrors the standard player response.
#define Player_ApplyLinkedContactResponse sub_08238580
void Player_ApplyLinkedContactResponse(struct Player *p, struct ContactCollider *source,
                                       struct ContactCollider *target)
{
    target->responseParameter = source->responseParameter;
    if (source->responseParameter > 200)
        target->responseParameter = 200;
    if (flagBits(&source->reactionFlags, 0x40000)) {
        p->contactSpecial = 1;
        p->facing = ContactCollider_GetImpactDirection(source, target);
        return;
    }
    if (p->tetherMode == 5) {
        target->flags |= 4;
        target->responseParameter = 0;
        return;
    }
    if (flagBits(&p->statusFlags, 0x8000)) {
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
            goto done;
        }
        p->contactDirection = dir;
    done:
        p->contactMagnitude = target->responseParameter;
        target->responseParameter = 0;
        target->flags |= 4;
        return;
    }
    p->contactSpecial = 0;
    p->facing = ContactCollider_GetImpactDirection(source, target);
}
