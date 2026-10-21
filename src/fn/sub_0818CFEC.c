#include "global.h"
#include "motion_0818.h"

extern const s16 gUnk_086149C4[];
s32 Div(s32, s32);

#define Armor_UpdatePartWaveZReturn sub_0818CFEC
void Armor_UpdatePartWaveZReturn(struct Motion0818Object *p)
{
    s32 wave;
    p->rotation += (u8)p->byte1DD;
    wave = (gUnk_086149C4[(0x100 - p->timer) & 0xff] >> 3) * p->turnDirection;
    p->x = p->startX + Div(p->dx * p->timer, 128);
    p->y = p->startY + Div(p->dy * p->timer, 128);
    p->z = p->startZ + Div(p->dz * p->timer, 128) + wave;
    p->timer--;
    if (p->timer == 0) {
        u8 one;
        p->rotation = 0x80;
        one = 1;
        p->byte11E = one;
        p->byte11F = one;
        p->flags119 |= one;
    }
}
