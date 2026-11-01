// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct Vec3 {
    s16 x, y, z;
};

struct NavProbe {
    u16 flags;
    u8 pad[0x1A];
    struct Vec3 pos;
};

void NavMesh_UpdateProbe(struct NavProbe *probe, struct Vec3 *target);
s32 Math_Atan2(s32, s32);
s32 Actor_MoveAlongAngle(struct Vec3 *pos, u32 angle, s32 dist);

static inline void Vec3_Sub(struct Vec3 *out, struct Vec3 *a, struct Vec3 *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

// Steers a nav probe: when the probe lies to the side of `angle` (relative to `target`),
// or `target` cannot move along `angle`, snaps to a 4-way direction and sets flag 2.
s32 sub_0821FB8C(struct NavProbe *probe, struct Vec3 *target, s32 angle, s32 dist)
{
    struct Vec3 d;
    s32 a, diff, ret;

    NavMesh_UpdateProbe(probe, target);
    probe->flags &= ~2;
    Vec3_Sub(&d, &probe->pos, target);
    a = Math_Atan2(d.x, d.z);
    diff = (a - angle) & 0xFF;
    if ((u32)(diff - 0x30) <= 0xA0) {
        ret = a + 0x20;
        ret &= 0xC0;
        probe->flags |= 2;
        return ret;
    }
    if (Actor_MoveAlongAngle(target, angle, dist) == 0) {
        if (diff <= 0x3F)
            a += 0x40;
        a &= 0xC0;
        {
            s32 flags = probe->flags | 2;
            probe->flags = flags;
        }
        do {
        } while (0);
        return a;
    }
    return angle;
}
