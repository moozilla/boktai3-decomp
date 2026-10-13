#include "global.h"

extern u8 *gUnk_02000474;
u32 ScriptProc_Start(u32 a, u32 b);

u32 sub_08232960(u8 *p)
{
    gUnk_02000474 = p;
    *(u32 *)(p + 0x18) = ScriptProc_Start(0x922E, 0x931E);
    *(u32 *)(p + 0x1C) = 0;
    return 0;
}
