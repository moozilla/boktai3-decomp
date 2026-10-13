#include "global.h"
#include "engine.h"

struct Pos081944C4 {
    u32 a, b;
};
extern const struct Pos081944C4 gUnk_0824F4E4;
extern const u8 gUnk_0824F4EC[], gUnk_0824F4F4[];
void sub_0821FF58(void *, s32, s32, u32, u32, s32);

void sub_081944C4(u8 *t)
{
    u8 *anim = t + 0x7120;
    u8 *sprite;
    u32 h;
    u8 *collider;
    u16 hz;
    u8 bz;
    AnimData_Load(anim, 0xc7f0);
    AnimData_SelectTiles(anim, 0x1a3);
    sprite = t + 0x713c;
    Sprite_Init(sprite, anim, 0);
    *(u32 *)sprite |= 1;
    hz = 0;
    bz = 0;
    *(u16 *)(sprite + 0x10) = hz;
    *(struct Pos081944C4 *)(t + 0x7158) = gUnk_0824F4E4;
    t[0x71bc] = bz;
    t[0x71bd] = bz;
    *(u16 *)(t + 0x71c0) = hz;
    h = *(u16 *)(t + 0x726e) >> 1;
    collider = t + 0x7168;
    Collider_Init(collider, *(u32 *)(t + 0x4c), 0x2001, 0, 16, (void *)gUnk_0824F4EC, (void *)gUnk_0824F4F4);
    sub_0821FF58(collider, h, 10, 0, 0x40000, 30);
    Collider_SetCallback(collider, NULL, NULL);
}
