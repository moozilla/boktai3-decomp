#include "global.h"

struct PoolActor {
    u8 pad0[0xC];
    void *anim;
    u8 pad10[0x5E - 0x10];
    u16 flags5E;
    u8 pad60[0x9C - 0x60];
    u16 timer9C;
    u8 padA0[0x15B - 0xA0];
    u8 count15B;
};
void AnimData_SelectPaletteBank(void *, u32);

void sub_08104748(struct PoolActor *o)
{
    u8 *q = (u8 *)o;
    if (o->timer9C) {
        o->flags5E |= 4;
        o->timer9C--;
    } else {
        o->flags5E &= ~4;
    }
    q += 0x15B;
    if (*q) {
        AnimData_SelectPaletteBank(o->anim, 0x192);
        (*q)--;
    } else {
        AnimData_SelectPaletteBank(o->anim, 0x1EA);
    }
}
