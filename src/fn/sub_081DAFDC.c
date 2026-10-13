#include "global.h"
struct Pair {
    u32 a, b;
};
void AnimData_Load(u8 *, u32);
void Sprite_Init(u8 *, u8 *, u32);
void AnimData_SelectTiles(u8 *, u32);
static inline void setFields(u8 *p, u32 bit)
{
    p[7] = bit;
    *(u32 *)p |= bit;
}
void sub_081DAFDC(u8 *sprite, u8 *resource, struct Pair *pos, s32 frame)
{
    AnimData_Load(resource, 0x8639);
    Sprite_Init(sprite, resource, 0);
    *(u16 *)(sprite + 0x10) = 0;
    frame += 0x116;
    AnimData_SelectTiles(resource, frame);
    *(struct Pair *)(sprite + 0x1c) = *pos;
    setFields(sprite, 1);
}
