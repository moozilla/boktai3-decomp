#include "global.h"
struct E { u8 pad[0x8b4]; u32 f; u32 g; u32 h; u8 pad2[0x908 - 0x8c0]; };
struct S { u8 pad[0x18]; struct E e[1]; };
struct E *sub_08120294(struct S *s, u32 k)
{
    s32 i;
    for (i = 0; i < 1; i++) {
        if (s->e[i].f != 0 && s->e[i].h == k) return &s->e[i];
    }
    return 0;
}
