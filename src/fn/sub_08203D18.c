#include "global.h"
s32 Anim_RestartSequence(void *, void *, u32, u32, u32);
s32 sub_08203CA0(u8 *);

s32 sub_08203D18(u8 *p)
{
    s16 *timer = (s16 *)(p + 0x12c);
    if (*timer > 0) {
        (*timer)--;
    } else {
        Anim_RestartSequence(p + 0x10c, *(void **)(p + 0x154), 0, p[0x132], 0);
        *(u32 *)(p + 0x108) = (u32)sub_08203CA0;
    }
    return 0;
}
