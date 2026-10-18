#define PhaseObject_EndApproachPause sub_080F2E6C
#define PhaseObject_ApproachTarget   sub_080F2D50
#define gPhaseDirectionSines         gUnk_086149C4
#include "global.h"

// Tentative family name: object with four phase entries and timed state callbacks.
struct PhaseTarget {
    u8 pad0[0x30];
    u16 x, y, z;
};
struct PhaseObject {
    u8 pad0[0x60];
    struct PhaseTarget *target;
    s16 velocityX;
    u16 unused66;
    s16 velocityZ;
    u8 pad6A[0x488 - 0x6a];
    u16 targetX, targetY, targetZ, pad48E;
    s16 dx, dy, dz;
    u8 pad496[0x4b1 - 0x496];
    u8 angle;
    u8 pad4B2[6];
    s32 timer;
    u8 pad4BC[0xb84 - 0x4bc];
    u16 x, y, z;
    u8 padB8A[0xcdc - 0xb8a];
    void (*callback)(struct PhaseObject *);
    u8 padCE0[0xd80 - 0xce0];
    u32 frame;
};
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
void sub_080F2E6C(struct PhaseObject *);
void sub_080F2D50(struct PhaseObject *p)
{
    s32 vx, vz, stepX, stepZ;
    p->targetX = p->target->x;
    p->targetY = p->target->y;
    p->targetZ = p->target->z;
    p->dx = p->targetX - p->x;
    p->dy = p->targetY - p->y;
    p->dz = p->targetZ - p->z;
    p->angle = Math_Atan2(p->dx, p->dz);
    vx = gUnk_086149C4[(p->angle + 0x40) & 0xff];
    vz = gUnk_086149C4[p->angle];
    if (vx >= 0)
        stepX = vx >> 10;
    else
        stepX = -(-vx >> 10);
    p->velocityX = stepX;
    if (vz >= 0)
        stepZ = vz >> 10;
    else
        stepZ = -(-vz >> 10);
    p->velocityZ = stepZ;
    p->x += p->velocityX;
    p->z += p->velocityZ;
    if (!(p->frame & 3)) {
        if (++p->timer == 0x50)
            p->callback = sub_080F2E6C;
    }
}
