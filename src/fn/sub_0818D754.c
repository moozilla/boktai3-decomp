#include "motion_0818.h"

extern u8 *gGameState;
extern const s16 gUnk_086149C4[];
u32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void SpinningApproach_UpdateSpinRamp(struct Motion0818Object *);
void SpinningApproach_InitMotion(struct Motion0818Object *p)
{
    u8 *owner = p->parent;
    p->angle = Math_Atan2(*(s16 *)(gGameState + 0x30) - p->x, *(s16 *)(gGameState + 0x34) - p->z);
    p->byte1DD = 0;
    p->callback = SpinningApproach_UpdateSpinRamp;
    p->a = 0;
    p->b = 0;
    p->c = 0;
    p->fx = *(u16 *)(owner + 0x5ac) - p->x;
    p->fy = *(u16 *)(owner + 0x5ae) - p->y;
    p->fz = *(u16 *)(owner + 0x5b0) - p->z;
    p->startX = p->x;
    p->startY = p->y;
    p->startZ = p->z;
    p->endX = p->x + Div(800 * gUnk_086149C4[(p->angle + 64) & 255], 4096);
    p->endY = *(u16 *)(gGameState + 0x32) + 220;
    p->endZ = p->z + Div(800 * gUnk_086149C4[p->angle], 4096);
    p->dx = p->endX - p->startX;
    p->dy = p->endY - p->startY;
    p->dz = p->endZ - p->startZ;
    p->timer = 0;
}
