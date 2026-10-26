#include "global.h"
#include "game_state.h"
struct Obj {
    u8 pad0[0x1c];
    s16 x, y, z;
    u8 pad22[0xf8];
    u8 angle;
    u8 pad11B[0x19];
    u16 endX, endY, endZ, pad13A, startX, startY, startZ, pad142, dx, dy, dz, pad14A;
    u16 fx, fy, fz, pad152, a, b, c, pad15A;
    u16 *target;
    u8 *parent;
    u8 pad164[0x18];
    void (*callback)(struct Obj *);
    u32 pad180, timer;
    u8 pad188[5];
    u8 byte1DD;
};
extern const s16 gUnk_086149C4[];
u32 Math_Atan2(s32, s32);
s32 Div(s32, s32);
void Sword_UpdateLungeSpinup(struct Obj *);
void Sword_InitSpinningLunge(struct Obj *p)
{
    u8 *owner = p->parent;
    p->angle = Math_Atan2(*(s16 *)(*(u8 **)(owner + 0x60) + 0x30) - p->x,
                          *(s16 *)(*(u8 **)(owner + 0x60) + 0x34) - p->z);
    p->byte1DD = 0;
    p->callback = Sword_UpdateLungeSpinup;
    p->a = 0;
    p->b = 0;
    p->c = 0;
    p->fx = p->target[0] - p->x;
    p->fy = p->target[1] - p->y;
    p->fz = p->target[2] - p->z;
    p->startX = p->x;
    p->startY = p->y;
    p->startZ = p->z;
    p->endX = p->x + Div(800 * gUnk_086149C4[(p->angle + 64) & 255], 4096);
    p->endY = *(u16 *)(*(u8 **)(owner + 0x60) + 0x32) + 220;
    p->endZ = p->z + Div(800 * gUnk_086149C4[p->angle], 4096);
    p->dx = p->endX - p->startX;
    p->dy = p->endY - p->startY;
    p->dz = p->endZ - p->startZ;
    p->timer = 0;
}
