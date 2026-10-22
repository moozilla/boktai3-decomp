#include "global.h"
struct S {
    u8 pad0[0x1a];
    u16 count;
    u8 pad1[0x74 - 0x1c];
    u8 flags[1];
};

bool32 sub_08138750(struct S *p)
{
    s32 i;
    for (i = 0; i < p->count; i++) {
        if (p->flags[i])
            return TRUE;
    }
    return FALSE;
}
