#include "global.h"

struct PalTrans081D4CE0 {
    u8 pad0[0x18];
    u32 w18;
    u32 index;
    u8 pad20[5];
    u8 b25;
    u16 h26;
    u8 pad28[0x9a4 - 0x28];
    u8 sprite[0x9c0 - 0x9a4];
    u16 h9c0, h9c2, h9c4;
    u8 pad9c6[0x9d0 - 0x9c6];
    u8 animData[0x9ec - 0x9d0];
    u16 origin[4];
    u8 pad9f4[0xa03 - 0x9f4];
    u8 bA03;
};

void PaletteTransition_StartDisplay(void *, u32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void CpuSet(const void *, void *, u32);

static inline void CpuSetArgs(void *dest, u32 control, void *src)
{
    CpuSet(src, dest, control);
}

int sub_081D4CE0(struct PalTrans081D4CE0 *p)
{
    u32 zero = 0;
    u32 fill;
    u8 *anim, *spr;
    u16 *org, *q;
    p->index = zero;
    p->bA03 = zero;
    p->b25 = zero;
    if (p->index > 0x52)
        p->index = zero;
    PaletteTransition_StartDisplay(p, p->index);
    anim = p->animData;
    AnimData_Load(anim, 0x1c1b);
    spr = p->sprite;
    Sprite_Init(spr, anim, 0x212);
    *(s16 *)(spr + 0x10) = 5;
    ((u8 *)p)[0x9ab] = 3;
    org = p->origin;
    p->h9c0 = org[0];
    q = &org[1];
    p->h9c2 = *q;
    p->h9c4 = *q;
    ((u8 *)p)[0x9ac] = 0x80;
    ((u8 *)p)[0x9ad] = 0x40;
    p->w18 = zero;
    p->h26 = zero;
    fill = zero;
    CpuSetArgs(org, 0x05000002, &fill);
    return 0;
}
