#include "global.h"

void SceneActorAnimation_SelectDirectionalFlags(void *, u32);

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

void sub_080EE148(u8 *p)
{
    if (TakeFlag(p + 0x2B2))
        SceneActorAnimation_SelectDirectionalFlags(p, 0x10);
    if (*(u8 *)(p + 0x2B3)) {
        u32 one = 1;
        if (*(u8 *)(p + 8) == 0) {
            u32 *q = *(u32 **)(p + 4);
            *q |= one;
        } else {
            u32 *q = (u32 *)((u8 *)*(u32 **)(p + 4) + 0x20);
            q[2] |= one;
        }
        *(u8 *)(p + 0x2B0) = 1;
    }
}
