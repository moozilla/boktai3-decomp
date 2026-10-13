#include "global.h"
void Collider_Init(u8 *, u32, u32, u32, u32, void *, void *);
void Collider_SetPosition(u8 *, void *, u32);
void Collider_SetCallback(u8 *, void *, u8 *);
void sub_0821FF58(u8 *, u32, u32, u32, u32, u32);
void sub_0802C9A0(u8 *p, void *a, void *b, void *fn)
{
    u8 *q = p + 0x6c;
    u32 flags = 0x2001;
    Collider_Init(q, 0, flags, 0, 16, a, b);
    Collider_SetPosition(q, p + 0xc, 0);
    Collider_SetCallback(q, fn, p);
    sub_0821FF58(q, *(u32 *)(p + 0x150), *(u32 *)(p + 0x154), *(u32 *)(p + 0x15c),
                 *(u32 *)(p + 0x160), *(u32 *)(p + 0x158));
    {
        u32 mask = 4;
        *(u32 *)(p + 0x50) |= mask;
    }
}
