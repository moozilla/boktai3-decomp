#include "global.h"
struct E { u8 pad[0x8b4]; u32 f; u8 pad2[0x908 - 0x8b8]; };
struct S { u8 pad[0x18]; struct E e[1]; };
struct E *sub_08120264(struct S *s)
{
    s32 i;
    for (i = 0; i < 1; i++) {
        if (s->e[i].f != 1) return &s->e[i];
    }
    return 0;
}
