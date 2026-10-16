#include "global.h"
void Collider_Init(u8 *, u32, u32, u32, u32, void *, void *);
void Collider_SetPosition(u8 *, void *, u32);
void Collider_SetCallback(u8 *, void *, u8 *);
void sub_0821FF7C(u8 *, u32, u32, u32);
s32 Collider_Refresh(u8 *);
void sub_0802CA1C(u8 *p, void *a, void *b, void *fn)
{
    u8 *q = p + 0xc0;
    u32 flags = 0x4001;
    Collider_Init(q, 0, flags, 0, 16, a, b);
    Collider_SetPosition(q, p + 0xc, 0);
    Collider_SetCallback(q, fn, p);
    sub_0821FF7C(q, *(u32 *)(p + 0x14c), *(u32 *)(p + 0x164), *(u32 *)(p + 0x168));
    Collider_Refresh(q);
    {
        u32 mask = 8;
        *(u32 *)(p + 0x50) |= mask;
    }
}
