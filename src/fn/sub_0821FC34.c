#include "global.h"

#ifdef NONMATCHING
// fuzzy: 97.5%
// Only the second flag set differs: ROM loads the flags into r1 and the constant 2 into r0.
// (`*(u16 *)probe |= 2` avoids an agbcc CSE leftover that `probe->flags |= 2` causes.)
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

// 8-direction variant of sub_0821FB8C: steers a nav probe; when the probe lies to the side of `angle`
// (relative to `target`), or `target` cannot move along `angle`, snaps to an 8-way direction and sets flag 2.
s32 sub_0821FC34(struct NavProbe *probe, struct Vec3 *target, s32 angle, s32 dist)
{
    struct Vec3 d;
    s32 a, diff, ret;

    NavMesh_UpdateProbe(probe, target);
    probe->flags &= ~2;
    Vec3_Sub(&d, &probe->pos, target);
    a = Math_Atan2(d.x, d.z);
    diff = (a - angle) & 0xFF;
    if ((u32)(diff - 0x18) <= 0xD0) {
        ret = a + 0x10;
        ret &= 0xE0;
        *(u16 *)probe |= 2;
        return ret;
    }
    if (Actor_MoveAlongAngle(target, angle, dist) == 0) {
        if (diff <= 0x1F)
            a += 0x20;
        a &= 0xE0;
        *(u16 *)probe |= 2;
        do {
        } while (0);
        return a;
    }
    return angle;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0821FC34);
#endif
