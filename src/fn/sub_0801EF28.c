#include "global.h"
#define SceneObjects_AddPrimaryId sub_0801EF28

int SceneObjects_HasPrimaryId(u8 *p, u16 id);

void sub_0801EF28(u8 *p)
{
    if (*(u16 *)(p + 0x2e) != 0 && !SceneObjects_HasPrimaryId(p, *(u16 *)(p + 0x2e))) {
        u32 i = p[0x1c] * 2;
        u8 *d = p + 0x1e;
        d += i;
        *(u16 *)d = *(u16 *)(p + 0x2e);
        p[0x1c]++;
    }
}
