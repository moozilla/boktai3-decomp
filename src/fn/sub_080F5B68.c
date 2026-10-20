#include "global.h"

#ifdef NONMATCHING
// fuzzy: 87.3%
// foundry draft (worker best 86.6%); what differs is mostly register allocation and instruction order
struct TargetMotion {
    u8 unknown0[6], rotation, unknown7[0x15];
    s16 x, y, z;
    u8 unknown22[0xF7], flags119, angle, unknown11B[3], state11E;
    s8 state11F;
    u8 unknown120[0x14];
    u16 endX, endY, endZ, pad13A, startX, startY, startZ, pad142;
    s16 dx, dy, dz;
    u16 pad14A, fx, fy, fz, pad152, a, b, c, pad15A;
    u16 *target;
    u8 unknown160[4];
    void (*callback164)(struct TargetMotion *);
    u8 unknown168[0x1C];
    s32 timer;
    u8 unknown188[3], done, unknown18C, spin;
};
struct TargetMotionCorrection {
    u32 x : 16;
    s32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};
s32 Div(s32, s32);
void IvyChild_UpdateAfterBob(struct TargetMotion *);
void TargetMotion_AdvanceFollowSegment(struct TargetMotion *p)
{
    struct TargetMotionCorrection correction;
    correction.x = p->target[0] - p->a;
    correction.y = p->target[1] - p->b;
    correction.z = p->target[2] - p->c;
    p->rotation += p->spin;
    p->x = correction.x + p->startX + Div(p->dx * p->timer, 100);
    p->y = correction.y + p->startY + Div(p->dy * p->timer, 100);
    p->z = correction.z + p->startZ + Div(p->dz * p->timer, 100);
    p->timer++;
    if (p->timer > 100) {
        p->done = 1;
        p->rotation = 128;
        p->state11E = 1;
        p->state11F = 1;
        {
            u32 one = 1;
            p->flags119 = one | p->flags119;
        }
        p->callback164 = IvyChild_UpdateAfterBob;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", TargetMotion_AdvanceFollowSegment);
#endif
